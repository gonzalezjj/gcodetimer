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

#include "GCodeProcessorBase.h"

#include "Utils.h"
#include "Config.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

GCodeProcessorBase::GCodeProcessorBase(ifstream *input) : input(input) {}

void GCodeProcessorBase::process_file() {
    static const float max_jerk_magnitude = Utils::get_euclidean_length(Config::get()->max_jerk);
    string line;
    COORDS pos = {0.0, 0.0, 0.0, 0.0};       // mm
    float rate = 0.0;
    while(!(*input).eof()) {
        float line_duration = 0.0;

        getline(*input, line);
        char_separator<char> sep(" ");
        tokenizer< char_separator<char> > tokens(line, sep);
        tokenizer< char_separator<char> >::iterator token_iter = tokens.begin();
        if (token_iter != tokens.end()) {
            if ((*token_iter).compare("G1") == 0) {     // Linear move
                COORDS target_pos;

                memcpy((void*)&target_pos, (void*)&pos, sizeof(COORDS));
                for(++token_iter; token_iter != tokens.end(); ++token_iter) {
                    char op;
                    float value;

                    sscanf((*token_iter).c_str(), "%c%f", &op, &value);
                    switch(op) {
                        case 'X': target_pos.x = value; break;
                        case 'Y': target_pos.y = value; break;
                        case 'Z': target_pos.z = value; break;
                        case 'E': target_pos.e = value; break;
                        case 'F': rate = value / 60; break;
                    }
                }

                COORDS movement = Utils::get_diff(target_pos, pos);
                float length = Utils::get_euclidean_length(movement);
                if (length > 0) {
                    float rate_speed_factor = Config::get()->speed_multiplier * rate / length;
                    COORDS target_speed_components = Utils::map(movement, [=](float c) { return c * rate_speed_factor; });

                    // Calculate the individual jerk components
                    float jerk_speed_factor = max_jerk_magnitude / length;
                    COORDS jerk_speed = Utils::map(movement, [=](float c) { return abs(c) * jerk_speed_factor; });

                    // Check if the components exceed the max jerk per component. If so, reduce all
                    // components by the required factor to comply with the max jerk settings
                    COORDS jerk_reduce_factor = Utils::map(jerk_speed, Config::get()->max_jerk, [](float jc, float mc) { return jc > mc ? mc / jc : 1.0; });
                    float jerk_multiplier = Utils::reduce(jerk_reduce_factor, [](float c, float factor) { return min (factor, c); }, 1.0);
                    jerk_speed = Utils::map(jerk_speed, [=](float c) { return c * jerk_multiplier * Config::get()->jerk_efficiency; });

                    // Calculate the magnitude of the final jerk vector
                    float jerk_magnitude = Utils::get_euclidean_length(jerk_speed);

                    // Calculate the speed delta for the acceleration and deceleration phase
                    COORDS speed_delta_components = Utils::map(target_speed_components, jerk_speed, [](float sc, float jc) { return Utils::pos(abs(sc) - jc); });

                    // Calculate the time required to complete the acceleration
                    const COORDS &max_accel = movement.e != 0.0 ? Config::get()->max_print_accel : Config::get()->max_move_accel;
                    COORDS accel_time_components = Utils::map(speed_delta_components, max_accel, [] (float sc, float ac) { return sc / ac; });
                    float accel_time = Utils::reduce(accel_time_components, [] (float c, float t) { return max(c, t); }, accel_time_components.x);

                    float accel_magnitude = 0.0;
                    if (accel_time > EPSILON) {
                        // Calculate the actual acceleration per component based on accel_time and speed_delta_components
                        COORDS accel = Utils::map(speed_delta_components, [=] (float c) { return c / accel_time; });

                        // Calculate the magnitude of the acceleration vector
                        accel_magnitude = Utils::get_euclidean_length(accel) * Config::get()->accel_efficiency;
                    } else {
                        accel_time = 0.0;
                    }

                    float speed_magnitude = Utils::get_euclidean_length(target_speed_components);

                    // Full acceleration (a*t^2 / 2) and deceleration (a*t^2 / 2) possible
                    if (length > (2 * jerk_magnitude + accel_magnitude * accel_time) * accel_time) {
                        line_duration = accel_time * 2 + (length - (2 * jerk_magnitude + accel_magnitude * accel_time) * accel_time) / speed_magnitude;

                    } else {
                        // l = 2 * (((t / 2) * a / 2 + js) * (t / 2)) = ((t / 4) * a + js) * t = t^2 * a / 4 + t * js
                        // t^2 * a / 4 + t * js - l = 0 => t = (-js + sqrt(js^2 + a*l)) / 2*(a / 4)
                        line_duration = (sqrt(jerk_magnitude * jerk_magnitude + accel_magnitude * length) - jerk_magnitude) / (accel_magnitude / 2);
                    }

                    memcpy((void*)&pos, (void*)&target_pos, sizeof(COORDS));
                }
            } else if ((*token_iter).compare("G28") == 0) {     // Home
                // We don't know how long this will take. Just set the position to 0 without adding any time
                if (++token_iter == tokens.end()) {
                    pos.x = 0;
                    pos.y = 0;
                    pos.z = 0;
                }
                for(token_iter; token_iter != tokens.end(); ++token_iter) {
                    char op;
                    float value;
                    sscanf((*token_iter).c_str(), "%c%f", &op, &value);
                    switch(op) {
                        case 'X': pos.x = value; break;
                        case 'Y': pos.y = value; break;
                        case 'Z': pos.z = value; break;
                    }
                }
            } else if ((*token_iter).compare("G92") == 0) {     // Reset coords
                if (++token_iter == tokens.end()) {
                    memset((void*)&pos, 0, sizeof(pos));
                }
                for(token_iter; token_iter != tokens.end(); ++token_iter) {
                    char op;
                    float value;
                    sscanf((*token_iter).c_str(), "%c%f", &op, &value);
                    switch(op) {
                        case 'X': pos.x = value; break;
                        case 'Y': pos.y = value; break;
                        case 'Z': pos.z = value; break;
                        case 'E': pos.e = value; break;
                    }
                }
            }
        }
        process_line(line, line_duration);
    }
}
