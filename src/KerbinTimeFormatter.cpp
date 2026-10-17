#include "KerbinTimeFormatter.hpp"
#include <cmath>
#include <cstdio>
#include <string>

std::string KerbinTimeFormatter::format(double seconds) const {
    double s = std::abs(seconds);

    const int years   = static_cast<int>(s / YEAR);   s = std::fmod(s, YEAR);
    const int days    = static_cast<int>(s / DAY);    s = std::fmod(s, DAY);
    const int hours   = static_cast<int>(s / HOUR);   s = std::fmod(s, HOUR);
    const int minutes = static_cast<int>(s / 60.0);   s = std::fmod(s, 60.0);

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%dy %dd %dh %dm %.1fs",
                  years, days, hours, minutes, s);
    return buf;
}
