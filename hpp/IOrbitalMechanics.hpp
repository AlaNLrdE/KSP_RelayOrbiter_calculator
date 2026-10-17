#pragma once
#include "CelestialBody.hpp"
#include <optional>

// (ISP) Pure orbital-mechanics computations — no database or UI knowledge.
class IOrbitalMechanics {
public:
    virtual ~IOrbitalMechanics() = default;

    [[nodiscard]] virtual double
    orbital_period(double semi_major_axis, double mu) const = 0;

    // Vis-viva: speed at radius r on an orbit with semi-major axis a.
    [[nodiscard]] virtual double
    vis_viva(double r, double semi_major_axis, double mu) const = 0;

    // Synchronous orbit altitude above surface, or nullopt if below surface/atm.
    [[nodiscard]] virtual std::optional<double>
    sync_orbit_altitude(const CelestialBody& body) const = 0;

    // Maximum safe orbital altitude (SOI radius - body radius),
    // or nullopt for bodies without an SOI limit (Kerbol).
    [[nodiscard]] virtual std::optional<double>
    soi_max_altitude(const CelestialBody& body) const = 0;
};
