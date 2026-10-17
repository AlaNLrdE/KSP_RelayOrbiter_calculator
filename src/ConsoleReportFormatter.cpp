#include "ConsoleReportFormatter.hpp"
#include "OrbitalResult.hpp"
#include <cstdio>
#include <string>

static constexpr int W = 62;

static void row(const char* label, const std::string& value,
                const char* unit = "", const char* warn = "") {
    std::printf("  %-32s: %s", label, value.c_str());
    if (unit && unit[0]) std::printf("  %s", unit);
    if (warn && warn[0]) std::printf("  %s", warn);
    std::putchar('\n');
}

static std::string fmt_km(double metres, int decimals = 2) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.*f", decimals, metres / 1000.0);
    return buf;
}

static std::string fmt_ms(double v) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.3f", v);
    return buf;
}

ConsoleReportFormatter::ConsoleReportFormatter(
    std::shared_ptr<ITimeFormatter> time_formatter)
    : time_formatter_(std::move(time_formatter)) {}

void ConsoleReportFormatter::print(const OrbitalResult& r) const {
    const std::string T_c_str = time_formatter_->format(r.T_c);
    const std::string T_e_str = time_formatter_->format(r.T_e);

    char ratio_buf[32];  std::snprintf(ratio_buf, sizeof(ratio_buf), "%.2f s", r.T_c);
    char ratio_e[32];    std::snprintf(ratio_e,   sizeof(ratio_e),   "%.2f s", r.T_e);
    char ratio_val[32];  std::snprintf(ratio_val,  sizeof(ratio_val), "%.6f", r.T_e / r.T_c);

    std::printf("\n");
    for (int i = 0; i < W; ++i) std::putchar('=');
    std::printf("\n  RESULTS — %s / target altitude %s km\n",
                r.body_name.c_str(), fmt_km(r.h_target).c_str());
    for (int i = 0; i < W; ++i) std::putchar('=');
    std::putchar('\n');

    // ── Circular orbit ────────────────────────────────────────────
    std::printf("\n  CIRCULAR ORBIT  (final satellite orbit)\n  ");
    for (int i = 0; i < W - 2; ++i) std::putchar('-');
    std::putchar('\n');
    row("Altitude",         fmt_km(r.h_target),        "km");
    row("Radius",           fmt_km(r.r_c),             "km");
    row("Orbital velocity", fmt_ms(r.v_c),             "m/s");
    {
        std::string period = T_c_str + "  (" + ratio_buf + ")";
        row("Orbital period", period);
    }

    // ── Elliptical parking orbit ──────────────────────────────────
    std::printf("\n  ELLIPTICAL PARKING ORBIT  (carrier vessel)\n  ");
    for (int i = 0; i < W - 2; ++i) std::putchar('-');
    std::putchar('\n');
    row("Apoapsis altitude", fmt_km(r.h_apo),  "km  (= target)");
    row("Apoapsis radius",   fmt_km(r.r_apo),  "km");

    const char* peri_warn = "";
    if      (!r.surface_ok) peri_warn = "  BELOW SURFACE";
    else if (!r.atm_ok)     peri_warn = "  INSIDE ATMOSPHERE";
    const char* peri_ok = (r.surface_ok && r.atm_ok) ? "km  OK" : "km";
    row("Periapsis altitude", fmt_km(r.h_peri), peri_ok, peri_warn);
    row("Periapsis radius",   fmt_km(r.r_peri), "km");
    row("Semi-major axis",    fmt_km(r.a_e),    "km");
    {
        std::string period = T_e_str + "  (" + ratio_e + ")";
        row("Orbital period", period);
    }
    row("T_e / T_c ratio", std::string(ratio_val), "(target 0.666667)");

    // ── Velocities ────────────────────────────────────────────────
    std::printf("\n  VELOCITIES\n  ");
    for (int i = 0; i < W - 2; ++i) std::putchar('-');
    std::putchar('\n');
    row("Circular orbit (at apoapsis alt)", fmt_ms(r.v_c),    "m/s");
    row("Carrier at apoapsis",              fmt_ms(r.v_apo),  "m/s");
    row("Carrier at periapsis",             fmt_ms(r.v_peri), "m/s");

    // ── Deployment procedure ──────────────────────────────────────
    std::printf("\n  DEPLOYMENT PROCEDURE\n  ");
    for (int i = 0; i < W - 2; ++i) std::putchar('-');
    std::putchar('\n');

    const std::string h_tgt_km = fmt_km(r.h_target);
    const std::string h_peri_km = fmt_km(r.h_peri);

    std::printf("   1. Circularize carrier at %s km.\n", h_tgt_km.c_str());
    std::printf("   2. Burn retrograde at apoapsis to set periapsis to %s km.\n", h_peri_km.c_str());
    std::printf("       (carrier is now in the elliptical parking orbit)\n");
    std::printf("   3. At apoapsis: release SAT-1.  SAT-1 circularizes.\n");
    std::printf("   4. Carrier completes 1 elliptical orbit  (%s).\n", T_e_str.c_str());
    std::printf("       -> SAT-1 is now 120 deg ahead at apoapsis.\n");
    std::printf("   5. At apoapsis: release SAT-2.  SAT-2 circularizes.\n");
    std::printf("   6. Carrier completes 1 more elliptical orbit  (%s).\n", T_e_str.c_str());
    std::printf("       -> SAT-1 at 120 deg, SAT-2 at 240 deg ahead.\n");
    std::printf("   7. At apoapsis: release SAT-3.  SAT-3 circularizes.\n");
    std::printf("   8. Constellation complete: 3 satellites, 120 deg spacing.\n");

    if (!r.surface_ok || !r.atm_ok) {
        std::printf("\n  ");
        for (int i = 0; i < W - 2; ++i) std::putchar('!');
        std::putchar('\n');
        if (!r.surface_ok) {
            std::printf("  Periapsis is underground!\n");
            std::printf("     Minimum target altitude (above surface): %.1f km\n",
                        r.min_h_surface / 1000.0);
        } else {
            std::printf("  Periapsis is inside the atmosphere!\n");
            if (r.min_h_atm.has_value())
                std::printf("     Minimum target altitude (clear of atmosphere): %.1f km\n",
                            *r.min_h_atm / 1000.0);
        }
        std::printf("  ");
        for (int i = 0; i < W - 2; ++i) std::putchar('!');
        std::putchar('\n');
    }

    std::printf("\n");
    for (int i = 0; i < W; ++i) std::putchar('=');
    std::putchar('\n');
}
