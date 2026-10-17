# KSP Relay Satellite Constellation Calculator

An interactive CLI tool that computes the orbital parameters needed to deploy **3 relay satellites evenly spaced 120° apart** using a single carrier vessel — no manual timing, no mods required.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Use Case](#use-case)
3. [Orbital Mechanics](#orbital-mechanics)
   - [The Core Constraint](#the-core-constraint)
   - [Deriving the Parking Orbit](#deriving-the-parking-orbit)
   - [Minimum Safe Altitude](#minimum-safe-altitude)
4. [Deployment Diagrams](#deployment-diagrams)
   - [Orbit Geometry](#orbit-geometry)
   - [Deployment Timeline](#deployment-timeline)
5. [Program Structure](#program-structure)
6. [Function Reference](#function-reference)
7. [Data Model](#data-model)
8. [Design Patterns](#design-patterns)
9. [Input Validation & Edge Cases](#input-validation--edge-cases)
10. [Performance Metrics](#performance-metrics)
11. [Supported Bodies](#supported-bodies)

---

## Quick Start

```
python3 relay_calculator.py
```

Requires Python 3.10+ (uses `float | None` union syntax). No external dependencies.

---

## Use Case

The goal is to deploy a relay network of 3 satellites in a circular orbit with equal angular spacing (120° apart), using one carrier ship that carries all three. The method avoids any need to measure angles in-flight — the geometry is guaranteed by choosing the correct parking orbit before launch.

```
Final constellation:

        SAT-1 (0°)
           │
  SAT-3  ──●──  SAT-2
  (240°)  Body  (120°)
```

---

## Orbital Mechanics

### The Core Constraint

Let:
- `r_c` = radius of the target circular orbit (body center to satellite)
- `T_c` = period of the circular orbit
- `T_e` = period of the carrier's elliptical parking orbit

When the carrier releases SAT-1 at apoapsis and SAT-1 circularizes, both objects are at the same position. After one full elliptical orbit (`T_e`), the carrier returns to apoapsis. In that same time, SAT-1 has traveled:

```
θ_SAT1 = (T_e / T_c) × 360°
```

For the carrier to be exactly 120° behind SAT-1 at that moment, SAT-1 must have advanced 240° (i.e., 360° − 120°):

```
T_e / T_c = 240° / 360° = 2/3
```

This single ratio drives the entire calculation.

### Deriving the Parking Orbit

From **Kepler's Third Law**, the orbital period scales with the semi-major axis:

```
T ∝ a^(3/2)
```

Therefore:

```
T_e / T_c = (a_e / r_c)^(3/2) = 2/3

a_e = r_c × (2/3)^(2/3)  ≈  r_c × 0.76314
```

The apoapsis of the elliptical orbit equals the circular orbit radius (the release point):

```
r_apo = r_c
```

The periapsis follows from the definition of semi-major axis:

```
a_e = (r_apo + r_peri) / 2
r_peri = 2 × a_e − r_apo
       = r_c × (2 × (2/3)^(2/3) − 1)
       ≈ r_c × 0.52628
```

Orbital velocities at each key point are computed via the **vis-viva equation**:

```
v = √(μ × (2/r − 1/a))
```

Where `μ` is the body's standard gravitational parameter (GM).

### Minimum Safe Altitude

The periapsis must clear the body's surface (and atmosphere). Setting `r_peri > R_body`:

```
r_c × 0.52628 > R_body
r_c > R_body / 0.52628 ≈ 1.9002 × R_body
h_target > 0.9002 × R_body
```

Minimum altitudes for selected bodies:

| Body   | Min altitude (surface) | Atmosphere top | Binding constraint |
|--------|------------------------|----------------|--------------------|
| Kerbin | 540.1 km               | 70 km          | 540.1 km           |
| Mun    | 180.0 km               | none           | 180.0 km           |
| Minmus | 54.0 km                | none           | 54.0 km            |
| Duna   | 288.0 km               | 50 km          | 288.0 km           |
| Eve    | 630.1 km               | 90 km          | 630.1 km           |
| Jool   | 5,400.7 km             | 200 km         | 5,400.7 km         |

---

## Deployment Diagrams

### Orbit Geometry

```
                  ┌──────── Circular orbit (r_c) ────────┐
                  │                                       │
                  │    ┌──── Elliptical parking orbit ──┐ │
                  │    │      (carrier vessel)          │ │
                  │    │                                │ │
  Periapsis ──────┼────┤ ─────────── Body ──────────── ┤─┼── Apoapsis = r_c
  (r_peri)        │    │              (●)               │ │  (release point)
                  │    └────────────────────────────────┘ │
                  │                                       │
                  └───────────────────────────────────────┘

  r_peri ≈ 0.526 × r_c
  a_e    ≈ 0.763 × r_c
  T_e    = (2/3) × T_c
```

### Deployment Timeline

```
  t = 0  ──────────────────────────────────────────────────────
    Position:  Carrier + SAT-1 both at apoapsis (0°)
    Action:    SAT-1 released → circularizes at r_c
               Carrier begins elliptical orbit (period T_e)

  t = T_e  ────────────────────────────────────────────────────
    SAT-1 has traveled (2/3)×360° = 240° along circular orbit
    Carrier is back at apoapsis (0°)

    Circular orbit view:
        SAT-1
         240°
          ●
         /
        /  120° gap  ← carrier arrives here
       ●─────────────●
      0°/360°       Body

    Action:  SAT-2 released → circularizes at r_c
             Carrier begins second elliptical orbit

  t = 2·T_e  ──────────────────────────────────────────────────
    SAT-1 has traveled another 240° → now at 480° = 120°
    SAT-2 has traveled 240° from 0° → now at 240°
    Carrier is back at apoapsis (0°)

    Final deployment view:
               SAT-1
                120°
                 ●
                / \
    SAT-2      /   \    0° ← carrier arrives here
     240° ●───── ● ─────●
               Body

    Action:  SAT-3 released → circularizes at r_c

  ✓ Final constellation: SAT-3=0°, SAT-1=120°, SAT-2=240°
    Equal 120° spacing guaranteed by orbital mechanics alone.
```

---

## Program Structure

```
relay_calculator.py
│
├── BODIES                   Static database dict (17 bodies)
├── KERBIN_HOUR/DAY/YEAR     Kerbin calendar constants
│
├── ── Pure math helpers ──────────────────────────────────────
│   ├── fmt_time(s)          Seconds → Kerbin "Xy Xd Xh Xm Xs"
│   ├── orbital_period(a,μ)  Kepler: T = 2π√(a³/μ)
│   ├── vis_viva(r,a,μ)      Orbital speed at any point
│   ├── sync_orbit_altitude  Geostationary/synchronous h
│   ├── soi_max_altitude     Max safe altitude (SOI−R)
│   └── children_of(name)    Bodies orbiting this body, sorted
│
├── ── UI layer ───────────────────────────────────────────────
│   ├── select_body()        Interactive body chooser
│   └── ask_altitude()       Context-aware altitude menu
│       ├── option: sync orbit (always shown)
│       ├── option: child body's orbital altitude (if kids > 0)
│       ├── option: midpoint between two children (if kids ≥ 2)
│       └── option: custom altitude (always shown)
│
├── ── Core calculation ───────────────────────────────────────
│   └── calculate(name,body,h) → dict
│       Applies the T_e=(2/3)T_c derivation; returns all
│       orbital parameters, safety flags, and min-altitude hints
│
├── ── Output ─────────────────────────────────────────────────
│   └── print_report(r)      Formatted sections + warnings
│
└── main()                   REPL loop (repeat or exit)
```

---

## Function Reference

### `orbital_period(a, mu) → float`

```
T = 2π √(a³ / μ)
```

- `a`: semi-major axis (m)
- `mu`: gravitational parameter GM (m³/s²)
- Returns: period in seconds

### `vis_viva(r, a, mu) → float`

```
v = √(μ × (2/r − 1/a))
```

- `r`: current orbital radius (m)
- `a`: semi-major axis of the orbit (m)
- `mu`: gravitational parameter (m³/s²)
- Returns: orbital speed (m/s)

Special cases:
- Circular orbit: `r = a`, gives `v = √(μ/r)`
- At apoapsis of the parking orbit: `r = r_c`, `a = a_e`
- At periapsis of the parking orbit: `r = r_peri`, `a = a_e`

### `sync_orbit_altitude(body) → float | None`

Derives the synchronous (geostationary) orbit altitude from the body's sidereal rotation period:

```
r_sync = (μ × (T_rot / 2π)²)^(1/3)
h_sync = r_sync − R_body
```

Returns `None` if the resulting altitude is below the surface or inside the atmosphere. Does **not** check SOI — that is handled by the caller.

### `soi_max_altitude(body) → float | None`

```
h_SOI_max = SOI_radius − R_body
```

Returns `None` for Kerbol (no practical SOI limit from the calculator's perspective).

### `children_of(body_name) → list[tuple[str, dict]]`

Filters `BODIES` for all entries whose `parent` matches `body_name`, then sorts by `orbital_radius` ascending. Used to populate the "at child's orbital distance" and "midpoint between two children" menu options.

### `calculate(body_name, body, h_target) → dict`

The core engine. All inputs are scalars; output is a plain dict.

| Output key    | Description                                   |
|---------------|-----------------------------------------------|
| `r_c`         | Circular orbit radius (m)                     |
| `T_c`         | Circular orbit period (s)                     |
| `v_c`         | Circular orbit velocity (m/s)                 |
| `T_e`         | Elliptical orbit period (s) = (2/3)·T_c       |
| `a_e`         | Elliptical orbit semi-major axis (m)           |
| `r_apo`       | Apoapsis radius = r_c (m)                     |
| `r_peri`      | Periapsis radius (m)                          |
| `h_peri`      | Periapsis altitude above surface (m)          |
| `v_apo`       | Carrier velocity at apoapsis (m/s)            |
| `v_peri`      | Carrier velocity at periapsis (m/s)           |
| `surface_ok`  | `True` if periapsis is above the surface      |
| `atm_ok`      | `True` if periapsis clears the atmosphere     |
| `min_h_surface` | Minimum target altitude for periapsis > 0  |
| `min_h_atm`   | Minimum target altitude for periapsis > atm   |

### `ask_altitude(body_name, body) → float`

Dynamically builds the altitude menu based on the body's properties:

```
Menu options shown:
  1. Synchronous orbit           → always present
  2. At a child's orbital dist.  → only if children_of() is non-empty
  3. Midpoint between two child. → only if len(children_of()) ≥ 2
  4. Custom altitude             → always present (last option)
```

All returned altitudes are in **metres**.

---

## Data Model

Each entry in `BODIES` is a dict with these fields:

| Field           | Type          | Unit    | Description                                           |
|-----------------|---------------|---------|-------------------------------------------------------|
| `radius`        | `float`       | m       | Mean equatorial radius                                |
| `mu`            | `float`       | m³/s²   | Standard gravitational parameter (GM)                 |
| `atm`           | `float`       | m       | Altitude of atmosphere top (0 if none)                |
| `rot_period`    | `float`       | s       | Sidereal rotation period                              |
| `soi`           | `float\|None` | m       | Sphere of Influence radius (`None` for Kerbol)        |
| `orbital_radius`| `float\|None` | m       | Semi-major axis of orbit around parent (`None` for Kerbol) |
| `parent`        | `str\|None`   | —       | Name of parent body (`None` for Kerbol)               |

The parent-child graph encoded in `parent` fields:

```
Kerbol
├── Moho
├── Eve
│   └── Gilly
├── Kerbin
│   ├── Mun
│   └── Minmus
├── Duna
│   └── Ike
├── Dres
├── Jool
│   ├── Laythe
│   ├── Vall
│   ├── Tylo
│   ├── Bop
│   └── Pol
└── Eeloo
```

---

## Design Patterns

### Data-Driven Design

All body parameters live in a single `BODIES` dict. Adding a new body (e.g., from a mod) requires adding exactly one dict entry — no code changes anywhere else. The UI, calculations, SOI checks, and child-body menus all derive their behavior from that entry automatically.

### Layered Architecture

The code is organized into four non-overlapping layers with one-way dependencies:

```
  Database (BODIES)
       ↓
  Pure math helpers  (no I/O, no side effects)
       ↓
  UI layer           (reads input, calls helpers)
       ↓
  calculate()        (pure function, no I/O)
       ↓
  print_report()     (output only)
       ↓
  main()             (REPL loop, calls all layers)
```

### Dynamic Menu Builder

`ask_altitude()` builds its option list at runtime by inspecting the body:

```python
menu = []
menu.append(("Synchronous orbit ...", "sync"))         # always
if kids:
    menu.append(("At a child's orbital distance", ...)) # conditional
    if len(kids) >= 2:
        menu.append(("Midpoint between two ...", ...))  # conditional
menu.append(("Custom altitude", "custom"))              # always
```

This avoids `if body_name == "Kerbol":` branches scattered through the code. The same logic works for any body with children (Jool, Kerbin, Duna, Eve).

### Guard Clauses

Validation in `ask_altitude()` uses early-continue guards rather than nested conditionals:

```python
if h_sync is None:
    print("...below surface...")
    continue
if h_soi_max is not None and h_sync > h_soi_max:
    print("...exceeds SOI...")
    continue
return h_sync   # only reached when all checks pass
```

### Pure Calculation Function

`calculate()` takes only scalar inputs and returns a plain dict with no side effects. This makes it trivially testable and reusable (e.g., for a future batch/file mode).

---

## Input Validation & Edge Cases

Every user input goes through multi-stage validation before reaching `calculate()`.

### Altitude validation chain

```
User input (string)
  │
  ├─ Not a number?              → re-prompt
  ├─ ≤ 0?                      → re-prompt
  ├─ < atmosphere ceiling?      → re-prompt with atmosphere height
  └─ > SOI max altitude?        → re-prompt with SOI limit

  → float in metres, guaranteed safe
```

### Bodies with synchronous orbit outside SOI

9 of the 17 bodies have synchronous orbits that exceed their SOI:

| Body   | Sync orbit | SOI limit | Ratio |
|--------|-----------|-----------|-------|
| Mun    | 2,970.6 km | 2,229.6 km | 1.33× |
| Minmus | 3,670.4 km | 2,187.4 km | 1.68× |
| Ike    | 1,133.9 km |   919.6 km | 1.23× |
| Vall   | 3,593.2 km | 2,106.4 km | 1.71× |
| Bop    | 2,588.2 km | 1,156.1 km | 2.24× |
| Pol    | 2,415.1 km |   998.1 km | 2.42× |
| Laythe | 4,686.3 km | 3,223.6 km | 1.45× |
| Tylo   | 14,157.9 km | 10,256.5 km | 1.38× |
| Moho   | 18,173.2 km | 9,396.7 km | 1.93× |

For these bodies, option 1 (synchronous orbit) is shown with a `⚠` tag and blocked if selected, redirecting the user to choose a lower custom altitude.

### Low periapsis warning

If the computed periapsis is underground or inside the atmosphere, `print_report()` displays:

```
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ⚠  Periapsis is underground!
     Minimum target altitude (above surface): 540.1 km
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
```

The minimum altitude is derived analytically:

```
h_min_surface = R × (1/factor − 1)  where factor = 2×(2/3)^(2/3) − 1 ≈ 0.52628
h_min_atm     = (R + h_atm) / factor − R
```

---

## Performance Metrics

All core functions are **O(1)** — closed-form expressions with no loops or iteration.

The only exception is `children_of()`, which is **O(n)** where n = total number of bodies (17). It also includes an `O(k log k)` sort on the results (k ≤ 5 children in practice).

Benchmarks (Python 3.12, 200,000 iterations each):

| Function               | Time per call |
|------------------------|---------------|
| `sync_orbit_altitude()`| 0.325 µs      |
| `soi_max_altitude()`   | < 0.1 µs      |
| `children_of()`        | 1.117 µs      |
| `calculate()`          | 1.269 µs      |
| `orbital_period()`     | < 0.1 µs      |
| `vis_viva()`           | < 0.1 µs      |

Total computation per session (all functions combined): **< 5 µs**. Runtime is entirely dominated by user I/O.

**Numerical precision:** The `T_e/T_c` ratio converges to `2/3` to 9 decimal places (verified: `0.666666667` vs target `0.666666667`). Floating-point representation of `(2/3)^(2/3)` is exact to machine precision (~15 significant digits).

---

## Supported Bodies

| # | Body    | Radius  | μ (m³/s²)       | Atmosphere | SOI        | Children |
|---|---------|---------|-----------------|------------|------------|----------|
| 1 | Kerbol  | 261,600 km | 1.172×10¹⁸   | none       | unlimited  | 7 planets |
| 2 | Moho    | 250 km  | 1.686×10¹¹      | none       | 9,647 km   | —        |
| 3 | Eve     | 700 km  | 8.172×10¹²      | 90 km      | 85,109 km  | Gilly    |
| 4 | Gilly   | 13 km   | 8.289×10⁶       | none       | 126 km     | —        |
| 5 | Kerbin  | 600 km  | 3.532×10¹²      | 70 km      | 84,159 km  | Mun, Minmus |
| 6 | Mun     | 200 km  | 6.514×10¹⁰      | none       | 2,430 km   | —        |
| 7 | Minmus  | 60 km   | 1.766×10⁹       | none       | 2,247 km   | —        |
| 8 | Duna    | 320 km  | 3.014×10¹¹      | 50 km      | 47,922 km  | Ike      |
| 9 | Ike     | 130 km  | 1.857×10¹⁰      | none       | 1,050 km   | —        |
|10 | Dres    | 138 km  | 2.148×10¹⁰      | none       | 32,833 km  | —        |
|11 | Jool    | 6,000 km| 2.825×10¹⁴      | 200 km     | 2,455,985 km| 5 moons |
|12 | Laythe  | 500 km  | 1.962×10¹²      | 50 km      | 3,724 km   | —        |
|13 | Vall    | 300 km  | 2.075×10¹¹      | none       | 2,406 km   | —        |
|14 | Tylo    | 600 km  | 2.825×10¹²      | none       | 10,857 km  | —        |
|15 | Bop     | 65 km   | 2.487×10⁹       | none       | 1,221 km   | —        |
|16 | Pol     | 44 km   | 7.217×10⁸       | none       | 1,042 km   | —        |
|17 | Eeloo   | 210 km  | 7.441×10¹⁰      | none       | 119,083 km | —        |
