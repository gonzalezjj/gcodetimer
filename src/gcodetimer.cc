/**
 * gcodetimer
 *
 * Copyright © 2016 Juan Jose Gonzalez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <cmath>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>

#include <string>
#include <vector>

#include "cfgpath.h"

#include "Utils.h"
#include "GCodeProcessorBase.h"
#include "CmdLineParams.h"
#include "Config.h"
#include "versioninfo.h"

using namespace std;

class GCodeTimeEstimator : public GCodeProcessorBase {
protected:
    float estimated_time;

    virtual void process_line(string line, float line_duration) {
        estimated_time += line_duration;
    }

public:
    GCodeTimeEstimator(ifstream *input) : GCodeProcessorBase(input), estimated_time(0.0) {}

    void process_file() {
        estimated_time = 0;
        GCodeProcessorBase::process_file();
    }

    float get_estimated_time() {
        return estimated_time;
    }
};

class GCodeTimeDecorator : public GCodeProcessorBase {
protected:
    float total_time, current_time, previous_printed_time;
    ostream *output;

    virtual void process_line(string line, float line_duration) {
        // Echo the original line
        *output << line << endl;

        current_time += line_duration;
        float new_print_time = round(total_time - current_time);
        if (new_print_time != previous_printed_time) {
            previous_printed_time = new_print_time;
            *output << "M117 ETR ";
            Utils::format_time(output, new_print_time);
            *output << endl;
        }
    }

public:
    GCodeTimeDecorator(ifstream *input, ostream *output, float total_time) : GCodeProcessorBase(input), total_time(total_time), output(output), current_time(0.0) {
        previous_printed_time = round(total_time);
    }

    void process_file() {
        *output << "; ---" << endl;
        *output << "; Decorated with timestamps by " << Project_NAME << " " << Project_VERSION_STRING << endl;

        *output << "; Print acceleration settings (X,Y,Z,E) in mm/(s^2): ("
            << Config::get()->max_print_accel.x << ", " << Config::get()->max_print_accel.y << ", " << Config::get()->max_print_accel.z << ", " << Config::get()->max_print_accel.e << "), "
            << (int)round(Config::get()->accel_efficiency * 100) << "% avg efficiency" << endl;

        *output << "; Move acceleration settings (X,Y,Z) in mm/(s^2): ("
            << Config::get()->max_move_accel.x << ", " << Config::get()->max_move_accel.y << ", " << Config::get()->max_move_accel.z << "), "
            << (int)round(Config::get()->accel_efficiency * 100) << "% avg efficiency" << endl;

        *output << "; Max jerk settings (X,Y,Z,E) in mm/s: (" << Config::get()->max_jerk.x << ", " << Config::get()->max_jerk.y << ", " << Config::get()->max_jerk.z << ", " << Config::get()->max_jerk.e << "), "
            << (int)round(Config::get()->jerk_efficiency * 100) << "% avg efficiency" << endl;

        *output << "; ---" << endl << endl;

        *output << "M117 TTL ";
        Utils::format_time(output, total_time);
        *output << endl;

        GCodeProcessorBase::process_file();
    }
};


int main(int argc, char **argv) {
    CmdLineParams params;
    params.parse(argc, argv);
    if (!params.is_valid()) {
        params.print_usage(string(argv[0]));
        return 0;
    }

    if (params.get_create_config()) {
        Config::get()->save();
        cout << "Config saved to " << Config::get()->get_path() << endl;
    } else {
        for (vector<string>::const_iterator it = params.get_inputs().begin(); it != params.get_inputs().end(); ++it) {
            ifstream input (*it);

            GCodeTimeEstimator estimator = GCodeTimeEstimator(&input);
            estimator.process_file();

            if (params.get_info_only()) {
                cout << *it << " total time: ";
                Utils::format_time(&cout, round(estimator.get_estimated_time()));
                cout << endl;
            } else {
                input.close();
                input.open(*it);

                ofstream outputFile;
                ostream *output;
                if (params.get_use_stdout()) {
                    output = &cout;
                } else {
                    string output_name;
                    if (params.get_output().empty()) {
                        int pos = (*it).rfind(".");
                        if (pos == string::npos) {
                            output_name = *it + ".timed";
                        } else {
                            output_name = (*it).substr(0, pos) + ".timed" + (*it).substr(pos, (*it).size() - pos);
                        }
                    } else {
                        output_name = params.get_output();
                    }
                    outputFile.open(output_name);
                    output = &outputFile;
                }

                GCodeTimeDecorator decorator (&input, output, estimator.get_estimated_time());
                decorator.process_file();

                if (outputFile.is_open())
                    outputFile.close();
            }

            input.close();
        }
    }
    return 0;
}
