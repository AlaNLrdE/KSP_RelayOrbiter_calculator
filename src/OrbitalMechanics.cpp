#include "OrbitalMechanics.hpp"

// STUB — all functions return zero / nullopt until implementation step.

double OrbitalMechanics::orbital_period(double, double) const {
    return 0.0;
}

double OrbitalMechanics::vis_viva(double, double, double) const {
    return 0.0;
}

std::optional<double> OrbitalMechanics::sync_orbit_altitude(const CelestialBody&) const {
    return std::nullopt;
}

std::optional<double> OrbitalMechanics::soi_max_altitude(const CelestialBody&) const {
    return std::nullopt;
}
