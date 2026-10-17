#pragma once
#include "ITimeFormatter.hpp"

// Formats seconds into Kerbin calendar: Xy Xd Xh Xm X.Xs
// (6-hour day, 426-day year)
class KerbinTimeFormatter final : public ITimeFormatter {
public:
    [[nodiscard]] std::string format(double seconds) const override;

    static constexpr double HOUR = 3'600.0;
    static constexpr double DAY  = 6.0  * HOUR;
    static constexpr double YEAR = 426.0 * DAY;
};
