#include <gtest/gtest.h>
#include <cmath>
#include <numbers>
#include "OrbitalMechanics.hpp"
#include "TestFixtures.hpp"

// ── orbital_period() ──────────────────────────────────────────────────────────

TEST(OrbitalMechanics, KeplerFormula_T2_mu_equals_4pi2_a3) {
    OrbitalMechanics mech;
    const double a  = 3'463'334.0;
    const double mu = fixtures::make_kerbin().mu;
    const double T  = mech.orbital_period(a, mu);
    EXPECT_NEAR(T * T * mu,
                4.0 * std::numbers::pi * std::numbers::pi * a * a * a,
                T * T * mu * 1e-9);
}

TEST(OrbitalMechanics, PeriodScalesWithKeplerLaw) {
    OrbitalMechanics mech;
    const double mu = fixtures::make_kerbin().mu;
    const double a  = 1'000'000.0;
    const double T1 = mech.orbital_period(a, mu);
    const double T2 = mech.orbital_period(2.0 * a, mu);
    EXPECT_NEAR(T2 / T1, std::pow(2.0, 1.5), 1e-9);
}

TEST(OrbitalMechanics, KerbinKSOPeriodEqualsRotationPeriod) {
    OrbitalMechanics mech;
    const auto kerbin = fixtures::make_kerbin();
    // Sync orbit altitude is defined by rot_period; T at that orbit must match.
    const double r_sync =
        std::cbrt(kerbin.mu * std::pow(kerbin.rot_period / (2.0 * std::numbers::pi), 2.0));
    const double T = mech.orbital_period(r_sync, kerbin.mu);
    EXPECT_NEAR(T, kerbin.rot_period, 0.01);
}

TEST(OrbitalMechanics, EllipticalPeriodRatioIsTwoThirds) {
    OrbitalMechanics mech;
    const double mu  = fixtures::make_kerbin().mu;
    const double r_c = 3'463'334.0;
    const double a_e = r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);
    const double T_c = mech.orbital_period(r_c, mu);
    const double T_e = mech.orbital_period(a_e, mu);
    EXPECT_NEAR(T_e / T_c, 2.0 / 3.0, 1e-9);
}

// ── vis_viva() ────────────────────────────────────────────────────────────────

TEST(OrbitalMechanics, CircularOrbitIdentity) {
    OrbitalMechanics mech;
    const double mu = fixtures::make_kerbin().mu;
    const double r  = 3'463'334.0;
    EXPECT_NEAR(mech.vis_viva(r, r, mu), std::sqrt(mu / r), 1e-6);
}

TEST(OrbitalMechanics, PeriapsisSpeedGreaterThanApoapsis) {
    OrbitalMechanics mech;
    const double mu    = fixtures::make_kerbin().mu;
    const double r_c   = 3'463'334.0;
    const double a_e   = r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);
    const double r_peri= 2.0 * a_e - r_c;
    EXPECT_GT(mech.vis_viva(r_peri, a_e, mu), mech.vis_viva(r_c, a_e, mu));
}

TEST(OrbitalMechanics, ApoapsisSpeeedLessThanCircular) {
    OrbitalMechanics mech;
    const double mu  = fixtures::make_kerbin().mu;
    const double r_c = 3'463'334.0;
    const double a_e = r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);
    EXPECT_LT(mech.vis_viva(r_c, a_e, mu), std::sqrt(mu / r_c));
}

TEST(OrbitalMechanics, EnergyConservationAlongOrbit) {
    OrbitalMechanics mech;
    const double mu    = fixtures::make_kerbin().mu;
    const double r_c   = 3'463'334.0;
    const double a_e   = r_c * std::pow(2.0 / 3.0, 2.0 / 3.0);
    const double r_peri= 2.0 * a_e - r_c;
    const double E_ref = -mu / (2.0 * a_e);
    const double v_apo = mech.vis_viva(r_c,    a_e, mu);
    const double v_peri= mech.vis_viva(r_peri, a_e, mu);
    const double E_apo = v_apo  * v_apo  / 2.0 - mu / r_c;
    const double E_peri= v_peri * v_peri / 2.0 - mu / r_peri;
    EXPECT_NEAR(E_apo,  E_ref, std::abs(E_ref) * 1e-9);
    EXPECT_NEAR(E_peri, E_ref, std::abs(E_ref) * 1e-9);
}

// ── sync_orbit_altitude() ─────────────────────────────────────────────────────

TEST(OrbitalMechanics, SyncOrbitAltitudeKerbinApprox2863km) {
    OrbitalMechanics mech;
    auto h = mech.sync_orbit_altitude(fixtures::make_kerbin());
    ASSERT_TRUE(h.has_value());
    EXPECT_NEAR(*h, 2'863'334.0, 500.0);
}

TEST(OrbitalMechanics, SyncOrbitPeriodMatchesBodyRotation) {
    OrbitalMechanics mech;
    const auto kerbin = fixtures::make_kerbin();
    auto h = mech.sync_orbit_altitude(kerbin);
    ASSERT_TRUE(h.has_value());
    const double T = mech.orbital_period(kerbin.radius + *h, kerbin.mu);
    EXPECT_NEAR(T, kerbin.rot_period, 0.01);
}

TEST(OrbitalMechanics, SyncOrbitKerbolReturnsValue) {
    // Kerbol has no SOI limit; sync orbit should compute (not return nullopt)
    OrbitalMechanics mech;
    auto h = mech.sync_orbit_altitude(fixtures::make_kerbol());
    EXPECT_TRUE(h.has_value());
}

TEST(OrbitalMechanics, SyncOrbitAboveSurface) {
    OrbitalMechanics mech;
    const auto kerbin = fixtures::make_kerbin();
    auto h = mech.sync_orbit_altitude(kerbin);
    ASSERT_TRUE(h.has_value());
    EXPECT_GT(*h, kerbin.atm);  // above atmosphere too
}

// ── soi_max_altitude() ────────────────────────────────────────────────────────

TEST(OrbitalMechanics, SoiMaxAltitudeKerbolReturnsNullopt) {
    OrbitalMechanics mech;
    EXPECT_FALSE(mech.soi_max_altitude(fixtures::make_kerbol()).has_value());
}

TEST(OrbitalMechanics, SoiMaxAltitudeKerbinEqualsSOIMinusRadius) {
    OrbitalMechanics mech;
    const auto kerbin = fixtures::make_kerbin();
    auto h = mech.soi_max_altitude(kerbin);
    ASSERT_TRUE(h.has_value());
    EXPECT_NEAR(*h, *kerbin.soi - kerbin.radius, 1.0);
}

TEST(OrbitalMechanics, SoiMaxAltitudeMun) {
    OrbitalMechanics mech;
    const auto mun = fixtures::make_mun();
    auto h = mech.soi_max_altitude(mun);
    ASSERT_TRUE(h.has_value());
    EXPECT_NEAR(*h, *mun.soi - mun.radius, 1.0);
}
