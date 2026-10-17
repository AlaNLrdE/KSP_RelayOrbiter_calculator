#pragma once
#include <string>

class ITimeFormatter {
public:
    virtual ~ITimeFormatter() = default;
    [[nodiscard]] virtual std::string format(double seconds) const = 0;
};
