// Exports golden test vectors as JSON for ports of the calculator
// (e.g. the C# KSP mod), so they can be checked against these results.
//
// For every body in KspBodyDatabase it writes the body's data and the
// OrbitalResult for a set of target altitudes: the synchronous orbit (when
// valid), each child's orbital altitude, the midpoint of the two innermost
// children, and a few fixed custom altitudes.
//
// Usage:  make -C build vectors   (writes build/obj/golden_vectors.json)
#include "ConstellationCalculator.hpp"
#include "KspBodyDatabase.hpp"
#include "OrbitalMechanics.hpp"
#include <cstdio>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

static void num(const char* key, double v, bool comma = true) {
    std::printf("\"%s\": %.17g%s", key, v, comma ? ", " : "");
}

static void opt_num(const char* key, const std::optional<double>& v, bool comma = true) {
    if (v.has_value()) num(key, *v, comma);
    else std::printf("\"%s\": null%s", key, comma ? ", " : "");
}

int main() {
    KspBodyDatabase db;
    auto mech = std::make_shared<OrbitalMechanics>();
    ConstellationCalculator calc(mech);

    const auto bodies = db.all();
    std::printf("{\n  \"generator\": \"KSP_RelayOrbiter_calculator/tools/export_vectors.cpp\",\n");
    std::printf("  \"bodies\": [\n");
    for (std::size_t bi = 0; bi < bodies.size(); ++bi) {
        const auto& b = bodies[bi];
        const auto kids = db.children_of(b.name);

        // (label, altitude in metres)
        std::vector<std::pair<std::string, double>> cases;
        const auto h_sync = mech->sync_orbit_altitude(b);
        const auto h_soi  = mech->soi_max_altitude(b);
        if (h_sync.has_value() && *h_sync > 0.0 && (!h_soi.has_value() || *h_sync <= *h_soi))
            cases.emplace_back("sync", *h_sync);
        for (const auto& k : kids)
            cases.emplace_back("child:" + k.name, *k.orbital_radius - b.radius);
        if (kids.size() >= 2)
            cases.emplace_back("mid:" + kids[0].name + "+" + kids[1].name,
                               (*kids[0].orbital_radius + *kids[1].orbital_radius) / 2.0 - b.radius);
        for (double km : {10.0, 100.0, 250.0, 1000.0})
            cases.emplace_back("custom", km * 1000.0);

        std::printf("    {\n      ");
        std::printf("\"name\": \"%s\", ", b.name.c_str());
        num("radius", b.radius);
        num("mu", b.mu);
        num("atm", b.atm);
        num("rot_period", b.rot_period);
        opt_num("soi", b.soi);
        opt_num("orbital_radius", b.orbital_radius);
        if (b.parent.has_value()) std::printf("\"parent\": \"%s\",\n      ", b.parent->c_str());
        else                      std::printf("\"parent\": null,\n      ");
        opt_num("sync_altitude", h_sync);
        opt_num("soi_max_altitude", h_soi, false);
        std::printf(",\n      \"results\": [\n");
        for (std::size_t ci = 0; ci < cases.size(); ++ci) {
            const auto r = calc.calculate(b, cases[ci].second);
            std::printf("        { \"case\": \"%s\", ", cases[ci].first.c_str());
            num("h_target", r.h_target);
            num("r_c", r.r_c);
            num("T_c", r.T_c);
            num("v_c", r.v_c);
            num("T_e", r.T_e);
            num("a_e", r.a_e);
            num("r_apo", r.r_apo);
            num("h_apo", r.h_apo);
            num("r_peri", r.r_peri);
            num("h_peri", r.h_peri);
            num("v_apo", r.v_apo);
            num("v_peri", r.v_peri);
            std::printf("\"surface_ok\": %s, \"atm_ok\": %s, ",
                        r.surface_ok ? "true" : "false", r.atm_ok ? "true" : "false");
            num("min_h_surface", r.min_h_surface);
            opt_num("min_h_atm", r.min_h_atm, false);
            std::printf(" }%s\n", ci + 1 < cases.size() ? "," : "");
        }
        std::printf("      ]\n    }%s\n", bi + 1 < bodies.size() ? "," : "");
    }
    std::printf("  ]\n}\n");
    return 0;
}
