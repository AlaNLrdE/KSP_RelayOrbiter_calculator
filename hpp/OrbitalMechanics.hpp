#pragma once
#include "IOrbitalMechanics.hpp"

class OrbitalMechanics final : public IOrbitalMechanics {
public:
    [[nodiscard]] double
    orbital_period(double semi_major_axis, double mu) const override;

    [[nodiscard]] double
    vis_viva(double r, double semi_major_axis, double mu) const override;

    [[nodiscard]] std::optional<double>
    sync_orbit_altitude(const CelestialBody& body) const override;

    [[nodiscard]] std::optional<double>
    soi_max_altitude(const CelestialBody& body) const override;
};
