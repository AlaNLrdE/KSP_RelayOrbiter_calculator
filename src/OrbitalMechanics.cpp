#include "OrbitalMechanics.hpp"
#include <cmath>
#include <numbers>

double OrbitalMechanics::orbital_period(double semi_major_axis, double mu) const {
    return 2.0 * std::numbers::pi * std::sqrt(semi_major_axis * semi_major_axis * semi_major_axis / mu);
}

double OrbitalMechanics::vis_viva(double r, double semi_major_axis, double mu) const {
    return std::sqrt(mu * (2.0 / r - 1.0 / semi_major_axis));
}

std::optional<double> OrbitalMechanics::sync_orbit_altitude(const CelestialBody& body) const {
    const double t = body.rot_period / (2.0 * std::numbers::pi);
    const double r_sync = std::cbrt(body.mu * t * t);
    return r_sync - body.radius;
}

std::optional<double> OrbitalMechanics::soi_max_altitude(const CelestialBody& body) const {
    if (!body.soi.has_value())
        return std::nullopt;
    return *body.soi - body.radius;
}
