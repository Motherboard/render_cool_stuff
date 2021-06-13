#include "baserender.h"
#include <cmath>

using u_char = unsigned char;

my::Color my::rainbow(double x) {
    static constexpr double step = 1.0/6;
    x -= std::floor(x);
    if (x < 1 * step)
        return {255, 0, static_cast<u_char>(255*x/step), 255};
    if (x < 2 * step)
        return {static_cast<u_char>(255 - 255.0/step*(x - (1 * step))), 0, 255, 255};
    if (x < 3 * step)
        return {0, static_cast<u_char>(255.0/step*(x - (2 * step))), 255, 255};
    if (x < 4 * step)
        return {0, 255, static_cast<u_char>(255 - 255.0/step*(x - (3 * step))), 255};
    if (x < 5 * step)
        return {static_cast<u_char>(255.0/step*(x - (4 * step))), 255, 0, 255};
    return {255, static_cast<u_char>(255-255.0/step*(x - (5 * step))), 0, 255};
}


