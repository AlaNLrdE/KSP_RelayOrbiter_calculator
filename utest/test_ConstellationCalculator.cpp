#include <gtest/gtest.h>
#include <cmath>
#include <numbers>
#include "ConstellationCalculator.hpp"
#include "OrbitalMechanics.hpp"
#include "TestFixtures.hpp"

// All tests use a real OrbitalMechanics injected via DI (no mocks needed here).

static ConstellationCalculator make_calc() {
    return ConstellationCalculator(std::make_shared<OrbitalMechanics>());
}

// ── Basic geometry ─────────────────────────────────────────────────────────────

TEST(ConstellationCalculator, BodyNamePropagated) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_EQ(r.body_name, "Kerbin");
}

TEST(ConstellationCalculator, TargetAltitudePropagated) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_DOUBLE_EQ(r.h_target, 100'000.0);
}

TEST(ConstellationCalculator, CircularOrbitRadiusEqualsRadiusPlusAltitude) {
    auto calc = make_calc();
    const auto kerbin = fixtures::make_kerbin();
    const double h = 100'000.0;
    auto r = calc.calculate(kerbin, h);
    EXPECT_DOUBLE_EQ(r.r_c, kerbin.radius + h);
}

TEST(ConstellationCalculator, ApoapsisRadiusEqualsCircularRadius) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_DOUBLE_EQ(r.r_apo, r.r_c);
}

TEST(ConstellationCalculator, ApoapsisAltitudeEqualsTargetAltitude) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_DOUBLE_EQ(r.h_apo, r.h_target);
}

// ── Orbital period constraint ─────────────────────────────────────────────────

TEST(ConstellationCalculator, EllipticalPeriodIsTwoThirdsCircular) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_NEAR(r.T_e / r.T_c, 2.0 / 3.0, 1e-9);
}

// ── Elliptical orbit geometry ─────────────────────────────────────────────────

TEST(ConstellationCalculator, SemiMajorAxisFromKepler) {
    // a_e = r_c * (2/3)^(2/3)
    auto calc = make_calc();
    const double h   = 100'000.0;
    const double r_c = fixtures::make_kerbin().radius + h;
    auto r = calc.calculate(fixtures::make_kerbin(), h);
    const double expected_a_e = r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);
    EXPECT_NEAR(r.a_e, expected_a_e, 1.0);
}

TEST(ConstellationCalculator, PeriapsisRadiusIs2ae_minus_rc) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_NEAR(r.r_peri, 2.0 * r.a_e - r.r_c, 1.0);
}

TEST(ConstellationCalculator, PeriapsisAltitudeEqualsRperiMinusBodyRadius) {
    auto calc = make_calc();
    const auto kerbin = fixtures::make_kerbin();
    auto r = calc.calculate(kerbin, 100'000.0);
    EXPECT_NEAR(r.h_peri, r.r_peri - kerbin.radius, 1.0);
}

// ── Velocity calculations ──────────────────────────────────────────────────────

TEST(ConstellationCalculator, CircularVelocityFromVisViva) {
    auto calc = make_calc();
    const auto kerbin = fixtures::make_kerbin();
    const double h = 100'000.0;
    const double r_c = kerbin.radius + h;
    auto r = calc.calculate(kerbin, h);
    EXPECT_NEAR(r.v_c, std::sqrt(kerbin.mu / r_c), 1e-6);
}

TEST(ConstellationCalculator, ApoapsisVelocityLessThanCircular) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_LT(r.v_apo, r.v_c);
}

TEST(ConstellationCalculator, PeriapsisVelocityGreaterThanApoApsis) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_GT(r.v_peri, r.v_apo);
}

// ── Safety flags ───────────────────────────────────────────────────────────────

TEST(ConstellationCalculator, SurfaceOkWhenPeriapsisAboveSurface) {
    auto calc = make_calc();
    // h_target = 2000 km well above Kerbin surface (600 km radius)
    auto r = calc.calculate(fixtures::make_kerbin(), 2'000'000.0);
    EXPECT_TRUE(r.surface_ok);
}

TEST(ConstellationCalculator, SurfaceNotOkWhenPeriapsisBelowSurface) {
    auto calc = make_calc();
    // Very low orbit: periapsis will dip below surface
    auto r = calc.calculate(fixtures::make_kerbin(), 1.0);
    EXPECT_FALSE(r.surface_ok);
}

TEST(ConstellationCalculator, AtmOkWhenPeriapsisAboveAtmosphere) {
    auto calc = make_calc();
    // h_target = 2000 km; Kerbin atmosphere is 70 km
    auto r = calc.calculate(fixtures::make_kerbin(), 2'000'000.0);
    EXPECT_TRUE(r.atm_ok);
}

TEST(ConstellationCalculator, AtmNotOkWhenPeriapsisBelowAtmosphere) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 1.0);
    EXPECT_FALSE(r.atm_ok);
}

TEST(ConstellationCalculator, NoAtmBodyAtmOkIsAlwaysTrue) {
    auto calc = make_calc();
    // Mun has no atmosphere (atm == 0), so atm_ok should be true whenever
    // surface_ok is true.
    auto r = calc.calculate(fixtures::make_mun(), 2'000'000.0);
    if (r.surface_ok) {
        EXPECT_TRUE(r.atm_ok);
    }
}

// ── Minimum altitudes ─────────────────────────────────────────────────────────

TEST(ConstellationCalculator, MinHSurfaceIsPositive) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_GT(r.min_h_surface, 0.0);
}

TEST(ConstellationCalculator, MinHAtmPresentForAtmosphericBody) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_kerbin(), 100'000.0);
    EXPECT_TRUE(r.min_h_atm.has_value());
}

TEST(ConstellationCalculator, MinHAtmAbsentForAirlessBody) {
    auto calc = make_calc();
    auto r = calc.calculate(fixtures::make_mun(), 100'000.0);
    EXPECT_FALSE(r.min_h_atm.has_value());
}
