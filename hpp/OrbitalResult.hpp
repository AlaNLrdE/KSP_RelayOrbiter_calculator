#pragma once
#include <optional>
#include <string>

// Value object returned by IConstellationCalculator::calculate().
// All distances in metres, times in seconds, velocities in m/s.
struct OrbitalResult {
    std::string body_name;
    double h_target      = 0.0;  // target circular orbit altitude
    double r_c           = 0.0;  // circular orbit radius
    double T_c           = 0.0;  // circular orbit period
    double v_c           = 0.0;  // circular orbit velocity
    double T_e           = 0.0;  // elliptical parking orbit period
    double a_e           = 0.0;  // elliptical orbit semi-major axis
    double r_apo         = 0.0;  // apoapsis radius
    double h_apo         = 0.0;  // apoapsis altitude
    double r_peri        = 0.0;  // periapsis radius
    double h_peri        = 0.0;  // periapsis altitude
    double v_apo         = 0.0;  // carrier speed at apoapsis
    double v_peri        = 0.0;  // carrier speed at periapsis
    bool   surface_ok    = false; // periapsis clears the surface
    bool   atm_ok        = false; // periapsis clears the atmosphere
    double min_h_surface = 0.0;  // minimum h_target for periapsis > 0 (km)
    std::optional<double> min_h_atm; // minimum h_target for periapsis > atm (km)
};
