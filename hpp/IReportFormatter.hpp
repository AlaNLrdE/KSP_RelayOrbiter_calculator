#pragma once
#include "OrbitalResult.hpp"

class IReportFormatter {
public:
    virtual ~IReportFormatter() = default;
    virtual void print(const OrbitalResult& result) const = 0;
};
