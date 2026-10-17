#include "ConsoleReportFormatter.hpp"

ConsoleReportFormatter::ConsoleReportFormatter(
    std::shared_ptr<ITimeFormatter> time_formatter)
    : time_formatter_(std::move(time_formatter)) {}

// STUB — prints nothing until implementation step.
void ConsoleReportFormatter::print(const OrbitalResult&) const {}
