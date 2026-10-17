#!/usr/bin/env python3
"""
KSP Relay Satellite Constellation Calculator
Deploys 3 satellites 120° apart using a single elliptical parking orbit.

Orbital mechanics:
  For the carrier to arrive at apoapsis 120° behind the previous satellite
  on each successive orbit, the elliptical period must satisfy:
      T_ellipse = (2/3) * T_circular
  This gives:  a_ellipse = r_circular * (2/3)^(2/3)
               r_periapsis = 2*a_ellipse - r_apoapsis
"""

import math

# ── Body database ─────────────────────────────────────────────────────────────
# radius / atm / soi in meters, mu in m³/s², rot_period in seconds (sidereal)
# orbital_radius: semi-major axis of this body's orbit around its parent (meters)
# soi: None for Kerbol (effectively unlimited for this calculator)
BODIES = {
    "Kerbol":  {"radius": 261_600_000, "mu": 1.1723328e18, "atm": 0,       "rot_period": 432_000,      "soi": None,          "orbital_radius": None,          "parent": None},
    "Moho":    {"radius": 250_000,     "mu": 1.6860938e11, "atm": 0,       "rot_period": 1_210_000,    "soi": 9_646_663,     "orbital_radius": 5_263_138_304, "parent": "Kerbol"},
    "Eve":     {"radius": 700_000,     "mu": 8.1717302e12, "atm": 90_000,  "rot_period": 80_500,       "soi": 85_109_365,    "orbital_radius": 9_832_684_544, "parent": "Kerbol"},
    "Gilly":   {"radius": 13_000,      "mu": 8_289_450,    "atm": 0,       "rot_period": 28_255,       "soi": 126_123,       "orbital_radius": 31_500_000,    "parent": "Eve"},
    "Kerbin":  {"radius": 600_000,     "mu": 3.5316000e12, "atm": 70_000,  "rot_period": 21_549.425,   "soi": 84_159_286,    "orbital_radius": 13_599_840_256,"parent": "Kerbol"},
    "Mun":     {"radius": 200_000,     "mu": 6.5138398e10, "atm": 0,       "rot_period": 138_984.38,   "soi": 2_429_559,     "orbital_radius": 12_000_000,    "parent": "Kerbin"},
    "Minmus":  {"radius": 60_000,      "mu": 1.7658000e9,  "atm": 0,       "rot_period": 1_077_311.08, "soi": 2_247_428,     "orbital_radius": 47_000_000,    "parent": "Kerbin"},
    "Duna":    {"radius": 320_000,     "mu": 3.0136321e11, "atm": 50_000,  "rot_period": 65_517.859,   "soi": 47_921_949,    "orbital_radius": 20_726_155_264,"parent": "Kerbol"},
    "Ike":     {"radius": 130_000,     "mu": 1.8568369e10, "atm": 0,       "rot_period": 65_517.862,   "soi": 1_049_598,     "orbital_radius": 3_200_000,     "parent": "Duna"},
    "Dres":    {"radius": 138_000,     "mu": 2.1484489e10, "atm": 0,       "rot_period": 34_800,       "soi": 32_832_840,    "orbital_radius": 40_839_348_203,"parent": "Kerbol"},
    "Jool":    {"radius": 6_000_000,   "mu": 2.8252800e14, "atm": 200_000, "rot_period": 36_000,       "soi": 2_455_985_200, "orbital_radius": 68_773_560_320,"parent": "Kerbol"},
    "Laythe":  {"radius": 500_000,     "mu": 1.9620000e12, "atm": 50_000,  "rot_period": 52_980.879,   "soi": 3_723_645,     "orbital_radius": 27_184_000,    "parent": "Jool"},
    "Vall":    {"radius": 300_000,     "mu": 2.0748150e11, "atm": 0,       "rot_period": 105_962.09,   "soi": 2_406_401,     "orbital_radius": 43_152_000,    "parent": "Jool"},
    "Tylo":    {"radius": 600_000,     "mu": 2.8252800e12, "atm": 0,       "rot_period": 211_926.36,   "soi": 10_856_518,    "orbital_radius": 68_500_000,    "parent": "Jool"},
    "Bop":     {"radius": 65_000,      "mu": 2.4868349e9,  "atm": 0,       "rot_period": 544_507.4,    "soi": 1_221_060,     "orbital_radius": 128_500_000,   "parent": "Jool"},
    "Pol":     {"radius": 44_000,      "mu": 7.2170208e8,  "atm": 0,       "rot_period": 901_902.62,   "soi": 1_042_138,     "orbital_radius": 179_890_000,   "parent": "Jool"},
    "Eeloo":   {"radius": 210_000,     "mu": 7.4410815e10, "atm": 0,       "rot_period": 19_460,       "soi": 119_082_940,   "orbital_radius": 113_549_713_200,"parent": "Kerbol"},
}

# Kerbin time constants (6 h/day, 426 days/year)
KERBIN_HOUR = 3_600
KERBIN_DAY  = 6 * KERBIN_HOUR
KERBIN_YEAR = 426 * KERBIN_DAY

# ── Helpers ───────────────────────────────────────────────────────────────────

def fmt_time(seconds: float) -> str:
    s = abs(seconds)
    years   = int(s // KERBIN_YEAR);  s %= KERBIN_YEAR
    days    = int(s // KERBIN_DAY);   s %= KERBIN_DAY
    hours   = int(s // KERBIN_HOUR);  s %= KERBIN_HOUR
    minutes = int(s // 60);           s  = s % 60
    parts = []
    if years:   parts.append(f"{years}y")
    if days:    parts.append(f"{days}d")
    if hours:   parts.append(f"{hours}h")
    if minutes: parts.append(f"{minutes}m")
    parts.append(f"{s:.1f}s")
    return " ".join(parts)

def orbital_period(a: float, mu: float) -> float:
    return 2 * math.pi * math.sqrt(a**3 / mu)

def vis_viva(r: float, a: float, mu: float) -> float:
    return math.sqrt(mu * (2.0/r - 1.0/a))

def sync_orbit_altitude(body: dict) -> float | None:
    """
    Returns the synchronous orbit altitude in metres, or None if it falls
    on or below the surface (or inside the atmosphere).
    Does NOT check SOI — callers handle that separately.
    """
    T   = body["rot_period"]
    mu  = body["mu"]
    R   = body["radius"]
    r   = (mu * (T / (2 * math.pi)) ** 2) ** (1.0 / 3.0)
    h   = r - R
    min_safe = max(0.0, body["atm"])
    return h if h > min_safe else None

def soi_max_altitude(body: dict) -> float | None:
    """
    Returns the maximum safe orbital altitude (SOI radius − body radius)
    in metres, or None if the body has no SOI limit (Kerbol).
    """
    soi = body["soi"]
    return (soi - body["radius"]) if soi is not None else None

def children_of(body_name: str) -> list[tuple[str, dict]]:
    """Returns (name, body) pairs for all bodies that directly orbit body_name,
    sorted by orbital_radius ascending."""
    kids = [(n, b) for n, b in BODIES.items() if b["parent"] == body_name]
    kids.sort(key=lambda x: x[1]["orbital_radius"])
    return kids

# ── UI helpers ────────────────────────────────────────────────────────────────

def select_body() -> tuple[str, dict]:
    body_list = sorted(BODIES.keys())
    print("\nAvailable bodies:")
    for i, name in enumerate(body_list, 1):
        b = BODIES[name]
        atm_str    = f"  atm ↑{b['atm']//1000} km" if b["atm"] else ""
        parent_str = f"orbits {b['parent']}" if b["parent"] else "the star"
        print(f"  {i:2d}. {name:<10}  R={b['radius']/1000:.0f} km{atm_str}  ({parent_str})")
    print()
    while True:
        raw = input("Select body (number or name): ").strip()
        if raw.isdigit():
            idx = int(raw) - 1
            if 0 <= idx < len(body_list):
                name = body_list[idx]
                return name, BODIES[name]
        if raw in BODIES:
            return raw, BODIES[raw]
        print("  Invalid selection — enter a number or exact name.")

def ask_altitude(body_name: str, body: dict) -> float:
    """
    Builds a context-aware altitude menu:
      Always:   synchronous orbit, custom altitude
      If body has children: at a child's orbital distance, midpoint between two children
    All choices validated against atmosphere and SOI.
    """
    atm_km    = body["atm"] / 1000
    atm_hint  = f"  (atmosphere ends at {atm_km:.0f} km)" if body["atm"] else ""
    h_sync    = sync_orbit_altitude(body)
    h_soi_max = soi_max_altitude(body)
    kids      = children_of(body_name)
    child_word = "moon" if body["parent"] is not None else "planet"

    # ── Build ordered option list ─────────────────────────────────
    # Each entry: (label_text, action_key)
    menu = []

    if h_sync is None:
        sync_tag = "below surface / unavailable"
    elif h_soi_max is not None and h_sync > h_soi_max:
        sync_tag = f"{h_sync/1000:.2f} km  ⚠ exceeds SOI limit ({h_soi_max/1000:.2f} km)"
    else:
        sync_tag = f"{h_sync/1000:.2f} km"
    menu.append((f"Synchronous orbit  →  {sync_tag}", "sync"))

    if kids:
        menu.append((f"At a {child_word}'s orbital distance", "child_single"))
        if len(kids) >= 2:
            menu.append((f"Midpoint between two {child_word}s", "child_mid"))

    menu.append(("Custom altitude", "custom"))

    # ── Print menu ────────────────────────────────────────────────
    print(f"\nTarget circular orbit altitude above {body_name}{atm_hint}")
    for i, (label, _) in enumerate(menu, 1):
        print(f"  {i}. {label}")
    print()
    valid_keys = [str(i) for i in range(1, len(menu) + 1)]

    # ── Handle selection ──────────────────────────────────────────
    while True:
        choice = input(f"  Option [{'/'.join(valid_keys)}]: ").strip()
        if choice not in valid_keys:
            print(f"  Enter {', '.join(valid_keys)}.")
            continue

        _, action = menu[int(choice) - 1]

        # ── Sync orbit ────────────────────────────────────────────
        if action == "sync":
            if h_sync is None:
                print("  ⚠  Synchronous orbit is below the surface. Choose another option.")
                continue
            if h_soi_max is not None and h_sync > h_soi_max:
                print(
                    f"\n  ⚠  Synchronous orbit altitude ({h_sync/1000:.2f} km) exceeds "
                    f"{body_name}'s SOI limit ({h_soi_max/1000:.2f} km).\n"
                    f"     A satellite at this altitude would escape {body_name}'s gravity.\n"
                    f"     Please choose another option with an altitude below {h_soi_max/1000:.2f} km.\n"
                )
                continue
            print(f"  Using synchronous orbit altitude: {h_sync/1000:.2f} km")
            return h_sync

        # ── At a child's orbital distance ─────────────────────────
        if action == "child_single":
            print(f"\n  Select {child_word}:")
            for i, (n, b) in enumerate(kids, 1):
                h_child = (b["orbital_radius"] - body["radius"]) / 1000
                print(f"    {i}. {n:<10}  orbital altitude: {h_child:,.2f} km")
            while True:
                raw = input(f"    {child_word.capitalize()} [number]: ").strip()
                if raw.isdigit() and 1 <= int(raw) <= len(kids):
                    chosen_name, chosen_body = kids[int(raw) - 1]
                    h = chosen_body["orbital_radius"] - body["radius"]
                    print(f"  Using {chosen_name}'s orbital altitude: {h/1000:,.2f} km")
                    return float(h)
                print(f"    Enter a number between 1 and {len(kids)}.")

        # ── Midpoint between two children ─────────────────────────
        if action == "child_mid":
            print(f"\n  Select two {child_word}s to compute the midpoint altitude:")
            for i, (n, b) in enumerate(kids, 1):
                h_child = (b["orbital_radius"] - body["radius"]) / 1000
                print(f"    {i}. {n:<10}  orbital altitude: {h_child:,.2f} km")
            while True:
                raw = input(f"    First {child_word} [number]: ").strip()
                if raw.isdigit() and 1 <= int(raw) <= len(kids):
                    idx_a = int(raw) - 1
                    break
                print(f"    Enter a number between 1 and {len(kids)}.")
            while True:
                raw = input(f"    Second {child_word} [number]: ").strip()
                if raw.isdigit() and 1 <= int(raw) <= len(kids):
                    idx_b = int(raw) - 1
                    if idx_b != idx_a:
                        break
                    print(f"    Choose a different {child_word} than the first.")
                else:
                    print(f"    Enter a number between 1 and {len(kids)}.")
            name_a, body_a = kids[idx_a]
            name_b, body_b = kids[idx_b]
            r_mid = (body_a["orbital_radius"] + body_b["orbital_radius"]) / 2
            h = r_mid - body["radius"]
            print(f"  Midpoint between {name_a} and {name_b}: {h/1000:,.2f} km")
            return float(h)

        # ── Custom altitude ───────────────────────────────────────
        if action == "custom":
            while True:
                raw = input("  Altitude [km]: ").strip()
                try:
                    h = float(raw) * 1000
                except ValueError:
                    print("  Please enter a number.")
                    continue
                if h <= 0:
                    print("  Altitude must be positive.")
                    continue
                if h < body["atm"]:
                    print(f"  ⚠  That altitude is inside the atmosphere ({atm_km:.0f} km). Try higher.")
                    continue
                if h_soi_max is not None and h > h_soi_max:
                    print(
                        f"  ⚠  Altitude ({h/1000:.2f} km) exceeds {body_name}'s SOI limit "
                        f"({h_soi_max/1000:.2f} km). Enter a lower altitude."
                    )
                    continue
                return h

# ── Core calculation ──────────────────────────────────────────────────────────

def calculate(body_name: str, body: dict, h_target: float) -> dict:
    R   = body["radius"]
    mu  = body["mu"]
    atm = body["atm"]

    r_c = R + h_target
    T_c = orbital_period(r_c, mu)
    v_c = math.sqrt(mu / r_c)

    # T_e/T_c = 2/3  →  a_e = r_c * (2/3)^(2/3)
    T_e    = (2.0 / 3.0) * T_c
    a_e    = r_c * (2.0 / 3.0) ** (2.0 / 3.0)
    r_apo  = r_c
    r_peri = 2.0 * a_e - r_apo
    h_peri = r_peri - R
    h_apo  = h_target

    v_apo  = vis_viva(r_apo,  a_e, mu)
    v_peri = vis_viva(r_peri, a_e, mu)

    surface_ok = h_peri > 0
    atm_ok     = h_peri >= atm

    # Minimum target altitude so that periapsis clears surface / atmosphere
    factor       = 2.0 * (2.0 / 3.0) ** (2.0 / 3.0) - 1.0
    min_h_surface = (R / factor - R) / 1000
    min_h_atm     = ((R + atm) / factor - R) / 1000 if atm else None

    return {
        "body": body_name, "R": R, "mu": mu, "atm": atm,
        "h_target": h_target, "r_c": r_c, "T_c": T_c, "v_c": v_c,
        "T_e": T_e, "a_e": a_e,
        "r_apo": r_apo, "h_apo": h_apo,
        "r_peri": r_peri, "h_peri": h_peri,
        "v_apo": v_apo, "v_peri": v_peri,
        "surface_ok": surface_ok, "atm_ok": atm_ok,
        "min_h_surface": min_h_surface,
        "min_h_atm": min_h_atm,
    }

# ── Report ─────────────────────────────────────────────────────────────────────

def print_report(r: dict) -> None:
    W = 62

    def header(title):
        print(f"\n  {title}")
        print("  " + "─" * (W - 2))

    def row(label, value, unit="", warn=""):
        line = f"  {label:<32}: {value}"
        if unit: line += f"  {unit}"
        if warn: line += f"  {warn}"
        print(line)

    print()
    print("=" * W)
    print(f"  RESULTS — {r['body'].upper()} / target altitude {r['h_target']/1000:.2f} km")
    print("=" * W)

    header("CIRCULAR ORBIT  (final satellite orbit)")
    row("Altitude",         f"{r['h_target']/1000:.2f}", "km")
    row("Radius",           f"{r['r_c']/1000:.2f}",      "km")
    row("Orbital velocity", f"{r['v_c']:.3f}",           "m/s")
    row("Orbital period",   f"{fmt_time(r['T_c'])}",     f"({r['T_c']:.2f} s)")

    header("ELLIPTICAL PARKING ORBIT  (carrier vessel)")
    row("Apoapsis altitude", f"{r['h_apo']/1000:.2f}",  "km  (= target)")
    row("Apoapsis radius",   f"{r['r_apo']/1000:.2f}",  "km")

    peri_warn = ""
    if not r["surface_ok"]:
        peri_warn = "⚠  BELOW SURFACE"
    elif not r["atm_ok"]:
        peri_warn = "⚠  INSIDE ATMOSPHERE"
    peri_ok = "✓" if (r["surface_ok"] and r["atm_ok"]) else ""
    row("Periapsis altitude", f"{r['h_peri']/1000:.2f}", f"km  {peri_ok}", peri_warn)
    row("Periapsis radius",   f"{r['r_peri']/1000:.2f}", "km")
    row("Semi-major axis",    f"{r['a_e']/1000:.2f}",    "km")
    row("Orbital period",     f"{fmt_time(r['T_e'])}",   f"({r['T_e']:.2f} s)")
    row("T_e / T_c ratio",    f"{r['T_e']/r['T_c']:.6f}","(target 0.666667)")

    header("VELOCITIES")
    row("Circular orbit (at apoapsis alt)", f"{r['v_c']:.3f}",    "m/s")
    row("Carrier at apoapsis",              f"{r['v_apo']:.3f}",  "m/s")
    row("Carrier at periapsis",             f"{r['v_peri']:.3f}", "m/s")

    header("DEPLOYMENT PROCEDURE")
    steps = [
        f"Circularize carrier at {r['h_target']/1000:.2f} km.",
        f"Burn retrograde at apoapsis to set periapsis to {r['h_peri']/1000:.2f} km.",
        f"  (carrier is now in the elliptical parking orbit)",
        f"At apoapsis: release SAT-1.  SAT-1 circularizes.",
        f"Carrier completes 1 elliptical orbit  ({fmt_time(r['T_e'])}).",
        f"  → SAT-1 is now 120° ahead at apoapsis.",
        f"At apoapsis: release SAT-2.  SAT-2 circularizes.",
        f"Carrier completes 1 more elliptical orbit  ({fmt_time(r['T_e'])}).",
        f"  → SAT-1 at 120°, SAT-2 at 240° ahead.",
        f"At apoapsis: release SAT-3.  SAT-3 circularizes.",
        f"✓ Constellation complete: 3 satellites, 120° spacing.",
    ]
    step_num = 0
    for step in steps:
        if not step.startswith("  "):
            step_num += 1
            print(f"  {step_num:2d}. {step}")
        else:
            print(f"       {step.strip()}")

    if not r["surface_ok"] or not r["atm_ok"]:
        print()
        print("  " + "!" * (W - 2))
        if not r["surface_ok"]:
            print(f"  ⚠  Periapsis is underground!")
            print(f"     Minimum target altitude (above surface): {r['min_h_surface']:.1f} km")
        else:
            print(f"  ⚠  Periapsis is inside the atmosphere!")
            if r["min_h_atm"]:
                print(f"     Minimum target altitude (clear of atmosphere): {r['min_h_atm']:.1f} km")
        print("  " + "!" * (W - 2))

    print()
    print("=" * W)

# ── Entry point ───────────────────────────────────────────────────────────────

def main():
    print()
    print("=" * 62)
    print("  KSP — 3-Satellite Relay Constellation Calculator")
    print("  Method: elliptical parking orbit, T_e = (2/3) T_c")
    print("=" * 62)

    while True:
        body_name, body = select_body()
        h_target        = ask_altitude(body_name, body)
        result          = calculate(body_name, body, h_target)
        print_report(result)

        print()
        again = input("Calculate for another body/altitude? [y/N]: ").strip().lower()
        if again not in ("y", "yes"):
            break

    print("\nGood luck with the mission!\n")


if __name__ == "__main__":
    main()
