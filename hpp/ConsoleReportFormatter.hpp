#pragma once
#include "IReportFormatter.hpp"
#include "ITimeFormatter.hpp"
#include <memory>

// (DIP) Depends on ITimeFormatter — calendar system is swappable.
class ConsoleReportFormatter final : public IReportFormatter {
public:
    explicit ConsoleReportFormatter(std::shared_ptr<ITimeFormatter> time_formatter);
    void print(const OrbitalResult& result) const override;

private:
    std::shared_ptr<ITimeFormatter> time_formatter_;
};
