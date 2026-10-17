#!/usr/bin/env python3
"""Generate the to-scale orbit figures used by README.md.

Writes constellation.svg, orbit_geometry.svg and deployment.svg into the
directory given as the first argument (default: docs/images). Standard
library only.

Angles follow the usual math convention: 0° at the release point (apoapsis,
pointing right), increasing counter-clockwise in the direction of motion.
"""
import math
import sys
from pathlib import Path

# ── Geometry of the method ────────────────────────────────────────────────────
A_RATIO = (2 / 3) ** (2 / 3)        # a_e / r_c  ≈ 0.76314
PERI_RATIO = 2 * A_RATIO - 1        # r_peri / r_c ≈ 0.52629

# Example body drawn to scale: Kerbin with a 1000 km target orbit.
KERBIN_R, KERBIN_ATM, TARGET_H = 600.0, 70.0, 1000.0
BODY_RATIO = KERBIN_R / (KERBIN_R + TARGET_H)                  # 0.375
ATM_RATIO = (KERBIN_R + KERBIN_ATM) / (KERBIN_R + TARGET_H)    # 0.419

# ── Palette ───────────────────────────────────────────────────────────────────
INK = "#1F2A37"
MUTED = "#5B6B7C"
ORBIT = "#3B6EA8"
CARRIER = "#D9822B"
SAT = "#2E7D5B"
BODY = "#7FA7D1"
ATM = "#D6E6F5"
BG = "#FFFFFF"
FONT = "Helvetica, Arial, sans-serif"


def pt(cx, cy, r, deg):
    """Point at radius r and angle deg (counter-clockwise, y up)."""
    rad = math.radians(deg)
    return cx + r * math.cos(rad), cy - r * math.sin(rad)


def text(x, y, s, size=13, anchor="middle", color=INK, weight="normal", italic=False):
    style = ' font-style="italic"' if italic else ""
    return (f'<text x="{x:.1f}" y="{y:.1f}" font-size="{size}" text-anchor="{anchor}" '
            f'fill="{color}" font-weight="{weight}"{style}>{s}</text>')


def arc(cx, cy, r, start, end, color, width=1.5, dash=None, marker=None):
    """Counter-clockwise arc from start to end degrees."""
    x1, y1 = pt(cx, cy, r, start)
    x2, y2 = pt(cx, cy, r, end)
    large = 1 if (end - start) % 360 > 180 else 0
    d = f' stroke-dasharray="{dash}"' if dash else ""
    m = f' marker-end="url(#{marker})"' if marker else ""
    return (f'<path d="M {x1:.1f} {y1:.1f} A {r:.1f} {r:.1f} 0 {large} 0 {x2:.1f} {y2:.1f}" '
            f'fill="none" stroke="{color}" stroke-width="{width}"{d}{m}/>')


def defs():
    return f"""<defs>
  <marker id="arrow" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
    <path d="M 0 0 L 10 5 L 0 10 z" fill="{MUTED}"/>
  </marker>
  <marker id="arrow-orbit" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
    <path d="M 0 0 L 10 5 L 0 10 z" fill="{ORBIT}"/>
  </marker>
</defs>"""


def svg(width, height, body):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
            f'viewBox="0 0 {width} {height}" font-family="{FONT}">\n'
            f'<rect width="100%" height="100%" fill="{BG}"/>\n{defs()}\n'
            + "\n".join(body) + "\n</svg>\n")


def planet(cx, cy, rc, atmosphere=True):
    out = []
    if atmosphere:
        out.append(f'<circle cx="{cx}" cy="{cy}" r="{rc * ATM_RATIO:.1f}" fill="{ATM}"/>')
    out.append(f'<circle cx="{cx}" cy="{cy}" r="{rc * BODY_RATIO:.1f}" fill="{BODY}"/>')
    return out


def circular_orbit(cx, cy, rc):
    return f'<circle cx="{cx}" cy="{cy}" r="{rc:.1f}" fill="none" stroke="{ORBIT}" stroke-width="2"/>'


def parking_orbit(cx, cy, rc, width=2, dash="7 5"):
    """Ellipse with the body at its focus and apoapsis at angle 0 on the circle."""
    a = rc * A_RATIO
    c = a - rc * PERI_RATIO                    # focus-to-centre distance
    b = math.sqrt(a * a - c * c)
    ex = cx - c                                # centre lies toward periapsis
    return (f'<ellipse cx="{ex:.1f}" cy="{cy}" rx="{a:.1f}" ry="{b:.1f}" fill="none" '
            f'stroke="{CARRIER}" stroke-width="{width}" stroke-dasharray="{dash}"/>')


def satellite(x, y, label=None, dx=0, dy=-14, size=12, anchor="middle"):
    out = [f'<rect x="{x - 6:.1f}" y="{y - 6:.1f}" width="12" height="12" rx="2" '
           f'fill="{SAT}" stroke="{BG}" stroke-width="1.5"/>']
    if label:
        out.append(text(x + dx, y + dy, label, size=size, anchor=anchor, weight="bold", color=SAT))
    return out


def carrier(x, y, label="Carrier", dx=0, dy=24, size=12):
    out = [f'<polygon points="{x:.1f},{y - 9:.1f} {x + 9:.1f},{y:.1f} {x:.1f},{y + 9:.1f} {x - 9:.1f},{y:.1f}" '
           f'fill="{CARRIER}" stroke="{BG}" stroke-width="1.5"/>']
    if label:
        out.append(text(x + dx, y + dy, label, size=size, weight="bold", color=CARRIER))
    return out


# ── Figure 1: final constellation ─────────────────────────────────────────────
def constellation():
    w, h, cx, cy, rc = 500, 400, 230, 200, 130
    b = [text(w / 2, 28, "Final constellation: three satellites 120° apart", 15, weight="bold")]
    b += planet(cx, cy, rc)
    b.append(text(cx, cy + 5, "Body", 13, color=INK))
    b.append(circular_orbit(cx, cy, rc))
    # spacing arcs
    for start in (0, 120, 240):
        b.append(arc(cx, cy, rc * 0.72, start + 8, start + 112, MUTED, 1.2, "3 3"))
        lx, ly = pt(cx, cy, rc * 0.60, start + 60)
        b.append(text(lx, ly + 4, "120°", 12, color=MUTED))
    # direction of motion
    b.append(arc(cx, cy, rc + 22, 25, 55, ORBIT, 1.8, marker="arrow-orbit"))
    mx, my = pt(cx, cy, rc + 40, 45)
    b.append(text(mx + 6, my, "motion", 11, anchor="start", color=ORBIT, italic=True))
    labels = {0: ("SAT-3  (0°)", 14, 5, "start"),
              120: ("SAT-1  (120°)", -10, -16, "middle"),
              240: ("SAT-2  (240°)", -10, 26, "middle")}
    for ang, (lab, dx, dy, anchor) in labels.items():
        x, y = pt(cx, cy, rc, ang)
        b += satellite(x, y, lab, dx=dx, dy=dy, anchor=anchor)
    b.append(text(w / 2, h - 14, "Angles measured from the release point (apoapsis) in the direction of motion",
                  11, color=MUTED))
    return svg(w, h, b)


# ── Figure 2: orbit geometry to scale ─────────────────────────────────────────
def orbit_geometry():
    w, h, cx, cy, rc = 760, 470, 360, 235, 170
    a = rc * A_RATIO
    rp = rc * PERI_RATIO
    b = [text(w / 2, 28, "Parking orbit geometry (to scale)", 15, weight="bold")]
    b += planet(cx, cy, rc)
    b.append(circular_orbit(cx, cy, rc))
    b.append(parking_orbit(cx, cy, rc, width=2.2))
    # major axis
    b.append(f'<line x1="{cx - rp:.1f}" y1="{cy}" x2="{cx + rc:.1f}" y2="{cy}" stroke="{MUTED}" '
             f'stroke-width="1" stroke-dasharray="2 3"/>')
    b.append(f'<circle cx="{cx}" cy="{cy}" r="3" fill="{INK}"/>')
    b.append(text(cx, cy + 18, "Body (focus)", 11, color=INK))
    # r_c radius
    rx, ry = pt(cx, cy, rc, 60)
    b.append(f'<line x1="{cx}" y1="{cy}" x2="{rx:.1f}" y2="{ry:.1f}" stroke="{ORBIT}" stroke-width="1.4" '
             f'marker-end="url(#arrow-orbit)"/>')
    lx, ly = pt(cx, cy, rc * 0.9, 67)
    b.append(text(lx, ly, "r_c", 13, color=ORBIT, weight="bold"))
    # apsides
    ax = cx + rc
    b += carrier(ax, cy, label=None)
    b.append(text(ax + 16, cy - 20, "Apoapsis = r_c", 13, anchor="start", color=INK, weight="bold"))
    b.append(text(ax + 16, cy - 4, "release point", 12, anchor="start", color=MUTED))
    b.append(text(ax + 16, cy + 12, "satellites circularize here", 12, anchor="start", color=MUTED))
    px = cx - rp
    b.append(f'<circle cx="{px:.1f}" cy="{cy}" r="5" fill="{CARRIER}"/>')
    b.append(f'<line x1="{cx - rc:.1f}" y1="{cy}" x2="{cx - rc - 12:.1f}" y2="{cy}" stroke="{MUTED}" '
             f'stroke-width="1" stroke-dasharray="2 3"/>')
    b.append(text(cx - rc - 16, cy - 8, "Periapsis", 13, anchor="end", color=INK, weight="bold"))
    b.append(text(cx - rc - 16, cy + 8, "r_peri ≈ 0.526 · r_c", 12, anchor="end", color=MUTED))
    # 2·a_e dimension
    dy = cy + rc + 26
    b.append(f'<line x1="{px:.1f}" y1="{dy}" x2="{ax:.1f}" y2="{dy}" stroke="{MUTED}" stroke-width="1.2" '
             f'marker-start="url(#arrow)" marker-end="url(#arrow)"/>')
    for x in (px, ax):
        b.append(f'<line x1="{x:.1f}" y1="{cy + 6}" x2="{x:.1f}" y2="{dy + 6}" stroke="{MUTED}" '
                 f'stroke-width="0.8" stroke-dasharray="2 3"/>')
    b.append(text((px + ax) / 2, dy - 6, "2 · a_e  (a_e ≈ 0.763 · r_c)", 12, color=MUTED))
    # legend
    lx, ly = 600, 70
    b.append(f'<line x1="{lx}" y1="{ly}" x2="{lx + 28}" y2="{ly}" stroke="{ORBIT}" stroke-width="2"/>')
    b.append(text(lx + 36, ly + 4, "Target circular orbit", 12, anchor="start"))
    b.append(text(lx + 36, ly + 20, "period T_c", 11, anchor="start", color=MUTED))
    ly += 44
    b.append(f'<line x1="{lx}" y1="{ly}" x2="{lx + 28}" y2="{ly}" stroke="{CARRIER}" stroke-width="2" '
             f'stroke-dasharray="7 5"/>')
    b.append(text(lx + 36, ly + 4, "Carrier parking orbit", 12, anchor="start"))
    b.append(text(lx + 36, ly + 20, "period T_e = ⅔ · T_c", 11, anchor="start", color=MUTED))
    ly += 44
    b.append(f'<circle cx="{lx + 14}" cy="{ly}" r="8" fill="{ATM}"/>')
    b.append(f'<circle cx="{lx + 14}" cy="{ly}" r="5" fill="{BODY}"/>')
    b.append(text(lx + 36, ly + 4, "Body + atmosphere", 12, anchor="start"))
    b.append(text(w / 2, h - 12, "Drawn to scale for Kerbin (R = 600 km, atmosphere 70 km) with a 1000 km target "
                  "— periapsis 242 km clears the atmosphere", 11, color=MUTED))
    return svg(w, h, b)


# ── Figure 3: deployment sequence ─────────────────────────────────────────────
def deployment():
    pw, w, h, rc = 260, 1040, 380, 82
    panels = [
        ("t = 0", "Release SAT-1 at apoapsis",
         {"SAT-1": None}, True, "Carrier + SAT-1 at 0°"),
        ("t = T_e", "Release SAT-2",
         {"SAT-1": 240, "SAT-2": None}, True, "SAT-1 has moved 240°"),
        ("t = 2 · T_e", "Release SAT-3",
         {"SAT-1": 120, "SAT-2": 240, "SAT-3": None}, True, "SAT-1 at 120°, SAT-2 at 240°"),
        ("Done", "Constellation complete",
         {"SAT-1": 120, "SAT-2": 240, "SAT-3": 0}, False, "120° spacing, no angle measured"),
    ]
    b = [text(w / 2, 28, "Deployment sequence — one release per parking orbit", 15, weight="bold")]
    for i, (title, action, sats, has_carrier, note) in enumerate(panels):
        ox = i * pw
        cx, cy = ox + pw / 2, 190
        if i:
            b.append(f'<line x1="{ox}" y1="60" x2="{ox}" y2="{h - 30}" stroke="#E3E8EE" stroke-width="1"/>')
        b.append(text(cx, 62, title, 14, weight="bold"))
        b.append(text(cx, 80, action, 12, color=MUTED))
        b += planet(cx, cy, rc)
        b.append(circular_orbit(cx, cy, rc))
        if has_carrier:
            b.append(parking_orbit(cx, cy, rc, width=1.6))
        # 0° reference tick
        x0, y0 = pt(cx, cy, rc + 10, 0)
        x1, y1 = pt(cx, cy, rc + 18, 0)
        b.append(f'<line x1="{x0:.1f}" y1="{y0:.1f}" x2="{x1:.1f}" y2="{y1:.1f}" stroke="{MUTED}" stroke-width="1"/>')
        b.append(text(x1 + 4, y1 + 4, "0°", 10, anchor="start", color=MUTED))
        if i == 1:
            b.append(arc(cx, cy, rc * 0.68, 0, 240, MUTED, 1.2, "3 3", marker="arrow"))
            lx, ly = pt(cx, cy, rc * 0.48, 120)
            b.append(text(lx, ly + 4, "240°", 11, color=MUTED))
        if has_carrier:
            x, y = pt(cx, cy, rc, 0)
            b += carrier(x, y, label="Carrier", dx=-6, dy=26, size=11)
        for name, ang in sats.items():
            if ang is None:            # being released at the carrier's position
                x, y = pt(cx, cy, rc, 0)
                b += satellite(x - 1, y - 16, name, dx=24, dy=4, size=11)
                b.append(f'<line x1="{x:.1f}" y1="{y - 9:.1f}" x2="{x:.1f}" y2="{y - 10:.1f}" stroke="{SAT}"/>')
            else:
                x, y = pt(cx, cy, rc, ang)
                dx, dy, anchor = {0: (8, -12, "start"), 120: (-10, -14, "middle"),
                                  240: (-14, 24, "middle")}[ang]
                b += satellite(x, y, name, dx=dx, dy=dy, size=11, anchor=anchor)
        b.append(text(cx, h - 46, note, 12, color=INK))
    b.append(text(w / 2, h - 12, "Each parking orbit takes T_e = ⅔ T_c, so the last satellite released always "
                  "trails the carrier by 120°", 11, color=MUTED))
    return svg(w, h, b)


def main():
    out = Path(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).resolve().parents[1] / "images")
    out.mkdir(parents=True, exist_ok=True)
    for name, fn in (("constellation", constellation),
                     ("orbit_geometry", orbit_geometry),
                     ("deployment", deployment)):
        (out / f"{name}.svg").write_text(fn(), encoding="utf-8")
        print(f"wrote {out / name}.svg")


if __name__ == "__main__":
    main()
