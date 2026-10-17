"""
Unit tests for relay_calculator.py

Covers: fmt_time, orbital_period, vis_viva, sync_orbit_altitude,
        soi_max_altitude, children_of, calculate
"""

import math
import pytest

from relay_calculator import (
    BODIES,
    KERBIN_DAY, KERBIN_HOUR, KERBIN_YEAR,
    fmt_time,
    orbital_period,
    vis_viva,
    sync_orbit_altitude,
    soi_max_altitude,
    children_of,
    calculate,
)

# ── Shared body shortcuts ─────────────────────────────────────────────────────

KERBIN = BODIES["Kerbin"]
MUN    = BODIES["Mun"]
MINMUS = BODIES["Minmus"]
KERBOL = BODIES["Kerbol"]
JOOL   = BODIES["Jool"]
DUNA   = BODIES["Duna"]
MOHO   = BODIES["Moho"]
GILLY  = BODIES["Gilly"]


# ── fmt_time ──────────────────────────────────────────────────────────────────

class TestFmtTime:
    def test_zero(self):
        assert fmt_time(0) == "0.0s"

    def test_seconds_only(self):
        assert fmt_time(30) == "30.0s"

    def test_decimal_seconds(self):
        assert fmt_time(1.5) == "1.5s"

    def test_minutes_and_seconds(self):
        assert fmt_time(90) == "1m 30.0s"

    def test_hours_minutes_seconds(self):
        assert fmt_time(3661) == "1h 1m 1.0s"

    def test_one_kerbin_day(self):
        # KERBIN_DAY = 6 h = 21600 s; no hours/minutes left over
        assert fmt_time(KERBIN_DAY) == "1d 0.0s"

    def test_day_hour_minute_second(self):
        assert fmt_time(KERBIN_DAY + 3661) == "1d 1h 1m 1.0s"

    def test_one_kerbin_year(self):
        assert fmt_time(KERBIN_YEAR) == "1y 0.0s"

    def test_full_all_components(self):
        t = KERBIN_YEAR + KERBIN_DAY + KERBIN_HOUR + 61
        assert fmt_time(t) == "1y 1d 1h 1m 1.0s"

    def test_large_multi_year(self):
        result = fmt_time(3 * KERBIN_YEAR)
        assert result.startswith("3y")

    def test_hour_boundary(self):
        # Exactly 2 Kerbin hours
        assert fmt_time(2 * KERBIN_HOUR) == "2h 0.0s"


# ── orbital_period ────────────────────────────────────────────────────────────

class TestOrbitalPeriod:
    def test_kepler_formula(self):
        # T = 2π √(a³/μ)  →  T² μ = 4π² a³
        a  = 3_463_334.0
        mu = KERBIN["mu"]
        T  = orbital_period(a, mu)
        assert T**2 * mu == pytest.approx(4 * math.pi**2 * a**3, rel=1e-9)

    def test_kerbin_kso_period(self):
        # Synchronous orbit period must equal Kerbin's rotation period
        h   = sync_orbit_altitude(KERBIN)
        r_c = KERBIN["radius"] + h
        T   = orbital_period(r_c, KERBIN["mu"])
        assert T == pytest.approx(KERBIN["rot_period"], rel=1e-6)

    def test_period_scales_with_kepler(self):
        # Doubling semi-major axis → period multiplied by 2^(3/2)
        mu = KERBIN["mu"]
        a  = 1_000_000.0
        T1 = orbital_period(a, mu)
        T2 = orbital_period(2 * a, mu)
        assert T2 / T1 == pytest.approx(2 ** 1.5, rel=1e-9)

    def test_elliptical_to_circular_ratio(self):
        # Core invariant: a_e = r_c*(2/3)^(2/3)  →  T_e/T_c = 2/3
        mu  = KERBIN["mu"]
        r_c = 3_463_334.0
        a_e = r_c * (2.0 / 3.0) ** (2.0 / 3.0)
        T_c = orbital_period(r_c, mu)
        T_e = orbital_period(a_e, mu)
        assert T_e / T_c == pytest.approx(2.0 / 3.0, rel=1e-9)


# ── vis_viva ──────────────────────────────────────────────────────────────────

class TestVisViva:
    def test_circular_orbit_identity(self):
        # For r == a (circular), vis-viva reduces to v = √(μ/r)
        mu = KERBIN["mu"]
        r  = 3_463_334.0
        assert vis_viva(r, r, mu) == pytest.approx(math.sqrt(mu / r), rel=1e-9)

    def test_higher_speed_at_periapsis_than_apoapsis(self):
        mu     = KERBIN["mu"]
        r_apo  = 3_463_334.0
        a_e    = r_apo * (2.0 / 3.0) ** (2.0 / 3.0)
        r_peri = 2 * a_e - r_apo
        v_apo  = vis_viva(r_apo,  a_e, mu)
        v_peri = vis_viva(r_peri, a_e, mu)
        assert v_peri > v_apo

    def test_apoapsis_slower_than_circular(self):
        # At apoapsis of an elliptical orbit, v < circular orbit speed
        mu    = KERBIN["mu"]
        r_c   = 3_463_334.0
        a_e   = r_c * (2.0 / 3.0) ** (2.0 / 3.0)
        v_apo = vis_viva(r_c, a_e, mu)
        v_c   = math.sqrt(mu / r_c)
        assert v_apo < v_c

    def test_energy_conservation(self):
        # Specific orbital energy E = -μ/(2a) is constant along the orbit
        # Verified via vis-viva at two points: E = v²/2 - μ/r = -μ/(2a)
        mu     = KERBIN["mu"]
        r_c    = 3_463_334.0
        a_e    = r_c * (2.0 / 3.0) ** (2.0 / 3.0)
        r_peri = 2 * a_e - r_c
        E_ref  = -mu / (2 * a_e)
        E_apo  = vis_viva(r_c,    a_e, mu) ** 2 / 2 - mu / r_c
        E_peri = vis_viva(r_peri, a_e, mu) ** 2 / 2 - mu / r_peri
        assert E_apo  == pytest.approx(E_ref, rel=1e-9)
        assert E_peri == pytest.approx(E_ref, rel=1e-9)


# ── sync_orbit_altitude ───────────────────────────────────────────────────────

class TestSyncOrbitAltitude:
    def test_kerbin_kso_altitude(self):
        # KSO is a well-known value in KSP (~2863 km)
        h = sync_orbit_altitude(KERBIN)
        assert h == pytest.approx(2_863_334.0, rel=1e-4)

    def test_sync_period_equals_rotation(self):
        # The orbital period at the sync altitude must equal the body's rotation
        h   = sync_orbit_altitude(KERBIN)
        r   = KERBIN["radius"] + h
        T   = orbital_period(r, KERBIN["mu"])
        assert T == pytest.approx(KERBIN["rot_period"], rel=1e-6)

    def test_mun_returns_value(self):
        # Mun is tidally locked; sync orbit exists (though outside SOI)
        h = sync_orbit_altitude(MUN)
        assert h is not None
        assert h > 0

    def test_gilly_sync_above_surface(self):
        h = sync_orbit_altitude(GILLY)
        assert h is not None
        assert h > 0

    def test_all_bodies_return_float_or_none(self):
        for name, body in BODIES.items():
            result = sync_orbit_altitude(body)
            assert result is None or isinstance(result, float), \
                f"Unexpected type for {name}: {type(result)}"


# ── soi_max_altitude ──────────────────────────────────────────────────────────

class TestSoiMaxAltitude:
    def test_kerbol_returns_none(self):
        assert soi_max_altitude(KERBOL) is None

    def test_kerbin(self):
        expected = KERBIN["soi"] - KERBIN["radius"]
        assert soi_max_altitude(KERBIN) == pytest.approx(expected, rel=1e-9)

    def test_mun(self):
        expected = MUN["soi"] - MUN["radius"]
        assert soi_max_altitude(MUN) == pytest.approx(expected, rel=1e-9)

    def test_all_non_kerbol_bodies_return_numeric(self):
        for name, body in BODIES.items():
            if name == "Kerbol":
                continue
            result = soi_max_altitude(body)
            assert isinstance(result, (int, float)), f"{name} should have a numeric SOI max"
            assert result > 0, f"{name} SOI max altitude must be positive"

    def test_soi_max_is_soi_minus_radius(self):
        for name, body in BODIES.items():
            if body["soi"] is None:
                continue
            assert soi_max_altitude(body) == body["soi"] - body["radius"]


# ── children_of ───────────────────────────────────────────────────────────────

class TestChildrenOf:
    def test_kerbin_has_two_moons(self):
        kids = children_of("Kerbin")
        names = [n for n, _ in kids]
        assert set(names) == {"Mun", "Minmus"}

    def test_kerbin_moons_sorted_by_orbital_radius(self):
        kids = children_of("Kerbin")
        radii = [b["orbital_radius"] for _, b in kids]
        assert radii == sorted(radii)
        # Mun is closer than Minmus
        assert kids[0][0] == "Mun"
        assert kids[1][0] == "Minmus"

    def test_kerbol_has_seven_planets(self):
        kids = children_of("Kerbol")
        assert len(kids) == 7

    def test_kerbol_planets_in_order(self):
        expected = ["Moho", "Eve", "Kerbin", "Duna", "Dres", "Jool", "Eeloo"]
        names = [n for n, _ in children_of("Kerbol")]
        assert names == expected

    def test_jool_has_five_moons(self):
        kids = children_of("Jool")
        names = [n for n, _ in kids]
        assert set(names) == {"Laythe", "Vall", "Tylo", "Bop", "Pol"}

    def test_jool_moons_in_order(self):
        expected = ["Laythe", "Vall", "Tylo", "Bop", "Pol"]
        names = [n for n, _ in children_of("Jool")]
        assert names == expected

    def test_body_without_children_returns_empty(self):
        assert children_of("Moho")  == []
        assert children_of("Mun")   == []
        assert children_of("Eeloo") == []

    def test_duna_has_one_moon(self):
        kids = children_of("Duna")
        assert len(kids) == 1
        assert kids[0][0] == "Ike"


# ── calculate ─────────────────────────────────────────────────────────────────

class TestCalculate:

    # Core orbital mechanics invariants ─────────────────────────────────────

    def test_period_ratio_is_two_thirds(self):
        # T_e / T_c must equal exactly 2/3 — this is the fundamental invariant
        for name, body in BODIES.items():
            h = body["radius"] * 1.0  # arbitrary: use one body-radius as altitude
            if h < body["atm"]:
                h = body["atm"] + 10_000
            r = calculate(name, body, h)
            assert r["T_e"] / r["T_c"] == pytest.approx(2.0 / 3.0, rel=1e-9), \
                f"Period ratio failed for {name}"

    def test_semi_major_axis_formula(self):
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        expected_a_e = r["r_c"] * (2.0 / 3.0) ** (2.0 / 3.0)
        assert r["a_e"] == pytest.approx(expected_a_e, rel=1e-9)

    def test_periapsis_from_semi_major_axis(self):
        # r_peri = 2*a_e - r_apo (definition of semi-major axis)
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        assert r["r_peri"] == pytest.approx(2 * r["a_e"] - r["r_apo"], rel=1e-9)

    def test_apoapsis_equals_circular_radius(self):
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        assert r["r_apo"] == pytest.approx(r["r_c"], rel=1e-9)

    def test_circular_velocity(self):
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        expected_v_c = math.sqrt(KERBIN["mu"] / r["r_c"])
        assert r["v_c"] == pytest.approx(expected_v_c, rel=1e-9)

    def test_vis_viva_at_apoapsis(self):
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        expected = vis_viva(r["r_apo"], r["a_e"], KERBIN["mu"])
        assert r["v_apo"] == pytest.approx(expected, rel=1e-9)

    def test_vis_viva_at_periapsis(self):
        h = 2_863_334.0
        r = calculate("Kerbin", KERBIN, h)
        expected = vis_viva(r["r_peri"], r["a_e"], KERBIN["mu"])
        assert r["v_peri"] == pytest.approx(expected, rel=1e-9)

    # Safety flags ──────────────────────────────────────────────────────────

    def test_surface_ok_false_when_altitude_too_low(self):
        # At 400 km above Kerbin, periapsis dips underground
        r = calculate("Kerbin", KERBIN, 400_000)
        assert r["surface_ok"] is False
        assert r["h_peri"] < 0

    def test_surface_ok_true_atm_ok_false_in_atmosphere(self):
        # At 600 km, periapsis clears the surface but not the 70 km atmosphere
        r = calculate("Kerbin", KERBIN, 600_000)
        assert r["surface_ok"] is True
        assert r["atm_ok"] is False
        assert 0 < r["h_peri"] < KERBIN["atm"]

    def test_both_ok_when_altitude_high_enough(self):
        r = calculate("Kerbin", KERBIN, 750_000)
        assert r["surface_ok"] is True
        assert r["atm_ok"] is True
        assert r["h_peri"] > KERBIN["atm"]

    def test_no_atmosphere_body_atm_ok_equals_surface_ok(self):
        # Mun has no atmosphere: atm_ok and surface_ok should behave identically
        r_low  = calculate("Mun", MUN, 100_000)   # below min altitude
        r_high = calculate("Mun", MUN, 300_000)   # above min altitude
        assert r_low["surface_ok"]  == r_low["atm_ok"]
        assert r_high["surface_ok"] == r_high["atm_ok"]

    # Known numerical values (Kerbin KSO) ───────────────────────────────────

    def test_kerbin_kso_period(self):
        h = sync_orbit_altitude(KERBIN)
        r = calculate("Kerbin", KERBIN, h)
        assert r["T_c"] == pytest.approx(21549.425, rel=1e-5)

    def test_kerbin_kso_circular_velocity(self):
        h = sync_orbit_altitude(KERBIN)
        r = calculate("Kerbin", KERBIN, h)
        assert r["v_c"] == pytest.approx(1009.807, rel=1e-4)

    def test_kerbin_kso_periapsis_altitude(self):
        h = sync_orbit_altitude(KERBIN)
        r = calculate("Kerbin", KERBIN, h)
        # Periapsis should be ~1222.7 km above Kerbin
        assert r["h_peri"] / 1000 == pytest.approx(1222.7, rel=1e-3)

    def test_kerbin_kso_apoapsis_velocity(self):
        h = sync_orbit_altitude(KERBIN)
        r = calculate("Kerbin", KERBIN, h)
        assert r["v_apo"] == pytest.approx(838.584, rel=1e-4)

    # Min-altitude hints ────────────────────────────────────────────────────

    def test_min_h_surface_positive(self):
        r = calculate("Kerbin", KERBIN, 400_000)
        assert r["min_h_surface"] > 0

    def test_min_h_surface_is_binding_constraint(self):
        # Using exactly min_h_surface should make periapsis ≈ 0
        r_hint = calculate("Kerbin", KERBIN, 400_000)
        min_h  = r_hint["min_h_surface"] * 1000   # km → m
        r_min  = calculate("Kerbin", KERBIN, min_h)
        assert r_min["h_peri"] == pytest.approx(0.0, abs=1.0)  # within 1 m

    def test_min_h_atm_provided_when_body_has_atmosphere(self):
        r = calculate("Kerbin", KERBIN, 600_000)
        assert r["min_h_atm"] is not None
        assert r["min_h_atm"] > r["min_h_surface"]

    def test_min_h_atm_none_when_no_atmosphere(self):
        r = calculate("Mun", MUN, 100_000)
        assert r["min_h_atm"] is None
