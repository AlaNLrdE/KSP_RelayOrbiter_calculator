#pragma once
#include "CelestialBody.hpp"

// Inline factory functions for commonly used bodies.
// Tests that depend only on orbital math use these instead of KspBodyDatabase,
// keeping each test class narrowly focused.
namespace fixtures {

inline CelestialBody make_kerbin() {
    return {
        .name           = "Kerbin",
        .radius         = 600'000.0,
        .mu             = 3.5316e12,
        .atm            = 70'000.0,
        .rot_period     = 21'549.425,
        .soi            = 84'159'286.0,
        .orbital_radius = 13'599'840'256.0,
        .parent         = "Kerbol",
    };
}

inline CelestialBody make_mun() {
    return {
        .name           = "Mun",
        .radius         = 200'000.0,
        .mu             = 6.5138398e10,
        .atm            = 0.0,
        .rot_period     = 138'984.38,
        .soi            = 2'429'559.0,
        .orbital_radius = 12'000'000.0,
        .parent         = "Kerbin",
    };
}

inline CelestialBody make_kerbol() {
    return {
        .name           = "Kerbol",
        .radius         = 261'600'000.0,
        .mu             = 1.1723328e18,
        .atm            = 0.0,
        .rot_period     = 432'000.0,
        .soi            = std::nullopt,
        .orbital_radius = std::nullopt,
        .parent         = std::nullopt,
    };
}

} // namespace fixtures
