#include "ConstellationCalculator.hpp"
#include <cmath>

ConstellationCalculator::ConstellationCalculator(
    std::shared_ptr<IOrbitalMechanics> mechanics)
    : mechanics_(std::move(mechanics)) {}

OrbitalResult ConstellationCalculator::calculate(const CelestialBody& body,
                                                  double h_target) const {
    OrbitalResult r;
    r.body_name = body.name;
    r.h_target  = h_target;

    // Circular parking orbit
    r.r_c = body.radius + h_target;
    r.T_c = mechanics_->orbital_period(r.r_c, body.mu);
    r.v_c = mechanics_->vis_viva(r.r_c, r.r_c, body.mu);

    // Elliptical transfer orbit: T_e = (2/3) * T_c  =>  a_e = r_c * (2/3)^(2/3)
    r.T_e = r.T_c * (2.0 / 3.0);
    r.a_e = r.r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);

    // Apoapsis == circular orbit; periapsis from semi-major axis
    r.r_apo  = r.r_c;
    r.h_apo  = h_target;
    r.r_peri = 2.0 * r.a_e - r.r_c;
    r.h_peri = r.r_peri - body.radius;

    // Velocities via vis-viva
    r.v_apo  = mechanics_->vis_viva(r.r_apo,  r.a_e, body.mu);
    r.v_peri = mechanics_->vis_viva(r.r_peri, r.a_e, body.mu);

    // Safety flags
    r.surface_ok = r.h_peri > 0.0;
    r.atm_ok     = body.atm <= 0.0 || r.h_peri > body.atm;

    // Minimum h_target so periapsis just clears surface: r_peri >= radius
    // r_peri = 2*a_e - r_c = 2*r_c*(2/3)^(2/3) - r_c = r_c*(2*(2/3)^(2/3)-1)
    // We need r_peri = radius  =>  r_c = radius / (2*(2/3)^(2/3)-1)
    const double factor = 2.0 * std::pow(2.0 / 3.0, 2.0 / 3.0) - 1.0;
    r.min_h_surface = body.radius / factor - body.radius;

    if (body.atm > 0.0) {
        // r_peri must clear atmosphere: r_peri = radius + atm
        const double r_peri_min = body.radius + body.atm;
        // r_c_min = (r_peri_min + r_c_min*(2*(2/3)^(2/3)-1)) / 2*(2/3)^(2/3)
        // r_peri = 2*a_e - r_c  and  a_e = r_c*(2/3)^(2/3)
        // r_peri = r_c*(2*(2/3)^(2/3) - 1)  =>  r_c = r_peri / factor
        const double r_c_min = r_peri_min / factor;
        r.min_h_atm = r_c_min - body.radius;
    }

    return r;
}
