#pragma once
#include <optional>
#include <string>

// Plain data structure — no behaviour, only identity.
struct CelestialBody {
    std::string name;
    double radius;                        // m — equatorial radius
    double mu;                            // m³/s² — standard gravitational parameter
    double atm;                           // m — atmosphere ceiling (0 if none)
    double rot_period;                    // s — sidereal rotation period
    std::optional<double>      soi;           // m — SOI radius (nullopt for Kerbol)
    std::optional<double>      orbital_radius; // m — semi-major axis around parent
    std::optional<std::string> parent;        // name of parent body (nullopt for Kerbol)
};
