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

#ifndef __INCLUDE_UTILS_H__
#define __INCLUDE_UTILS_H__

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>

#include <functional>

#include <cmath>

#define EPSILON 0.0000005f

typedef struct _COORDS {
    float x, y, z, e;
} COORDS;

class Utils {
public:
    static inline void format_time(std::ostream *stream, float time) {
        int h = floor(time / 3600);
        int m = floor((time - 3600 * h) / 60);
        int s = floor(time - 3600 * h - 60 * m);
        *stream << std::setw(2) << std::setfill('0') << h << "h" << std::setw(2) << std::setfill('0') << m << "m" << std::setw(2) << std::setfill('0') << s << "s";
    }

    static inline float pos(float input) {
        return input > 0.0 ? input : 0.0;
    }

    static inline COORDS get_diff(const COORDS &a, const COORDS &b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z, a.e - b.e};
    }

    static inline float get_euclidean_length(const COORDS &coords) {
        return sqrt(coords.x * coords.x + coords.y * coords.y + coords.z * coords.z + coords.e * coords.e);
    }

    static inline COORDS map(COORDS input, std::function<float (float)> op) {
        return { op(input.x), op(input.y), op(input.z), op(input.e) };
    }


    static inline COORDS map(COORDS a, COORDS b, std::function<float (float, float)> op) {
        return { op(a.x, b.x), op(a.y, b.y), op(a.z, b.z), op(a.e, b.e) };
    }

    static inline float reduce(COORDS input, std::function<float (float, float)> op, float acc) {
        acc = op(input.x, acc);
        acc = op(input.y, acc);
        acc = op(input.z, acc);
        acc = op(input.e, acc);
        return acc;
    }
};


#endif //__INCLUDE_UTILS_H__
