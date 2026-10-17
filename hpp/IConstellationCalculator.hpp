#pragma once
#include "CelestialBody.hpp"
#include "OrbitalResult.hpp"

// Applies the T_e = (2/3) T_c logic to derive a full parking-orbit solution.
class IConstellationCalculator {
public:
    virtual ~IConstellationCalculator() = default;

    [[nodiscard]] virtual OrbitalResult
    calculate(const CelestialBody& body, double h_target) const = 0;
};
