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

#ifndef __INCLUDE_CMDLINEPARAMS_H__
#define __INCLUDE_CMDLINEPARAMS_H__

#include <vector>
#include <string>

class CmdLineParams {
protected:
    enum State {
        STATE_MAIN,
        STATE_OUTPUT
    };

    std::vector<std::string> inputs;
    bool info_only;
    bool use_stdout;
    std::string output;
    bool create_config;

public:
    CmdLineParams();

    const std::vector<std::string> & get_inputs();
    const std::string & get_output();
    bool get_info_only();
    bool get_use_stdout();
    bool get_create_config();

    bool is_valid();

    void print_usage(std::string programName);

    void parse(int argc, char **argv);
};

#endif //__INCLUDE_CMDLINEPARAMS_H__
