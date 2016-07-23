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

#include "CmdLineParams.h"

#include <iostream>
#include <cstring>

#include "versioninfo.h"

using namespace std;

CmdLineParams::CmdLineParams() : inputs(vector<string> ()), info_only(false), use_stdout(false), create_config(false), output() {}

const vector<string> & CmdLineParams::get_inputs() {
    return inputs;
}

const string & CmdLineParams::get_output() { return output; }
bool CmdLineParams::get_info_only() { return info_only; }
bool CmdLineParams::get_use_stdout() { return use_stdout; }
bool CmdLineParams::get_create_config() { return create_config; }

bool CmdLineParams::is_valid() {
    return (create_config && inputs.size() == 0) || (inputs.size() > 0 && ((output.empty() && !use_stdout) || inputs.size() == 1));
}

void CmdLineParams::print_usage(string programName) {
    cout << Project_NAME << " version " << Project_VERSION_STRING << endl << endl;
    cout << "Usage: " << programName << " ([-i|--info] [-o|--output <output file>] [-s|--stdout] <gcode file> [<gcode file> ...] | --create-config)" << endl;
    cout << "  -i, --info: Only print the estimated time for each file, do not generate gcode" << endl;
    cout << "  -o, --output: Sets the output filename. Can only be used with a single input file" << endl;
    cout << "  -s, --stdout: Prints the generated gcode to stdout instead of saving it to a file." << endl
            << "                   Can only be used with a single input file. -o will be ignored" << endl;
    cout << "  --create-config: Generates or completes the config file with any missing defaults" << endl;
    cout << endl;
    cout << "If -o is not specified, the program will create a file of the new name with a '.timed' suffix" << endl
            << "  for each input file" << endl;
}

void CmdLineParams::parse(int argc, char **argv) {
    int state = STATE_MAIN;

    for (int i = 1; i < argc; i++) {
        switch (state) {
            case STATE_MAIN:
                if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
                    state = STATE_OUTPUT;
                } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--info") == 0) {
                    info_only = true;
                } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stdout") == 0) {
                    use_stdout = true;
                } else if (strcmp(argv[i], "--create-config") == 0) {
                    create_config = true;
                } else {
                    inputs.push_back(string(argv[i]));
                }
                break;
            case STATE_OUTPUT:
                output = string(argv[i]);
                state = STATE_MAIN;
                break;
        }
    }
}
