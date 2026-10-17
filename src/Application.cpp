#include "Application.hpp"
#include "CelestialBody.hpp"
#include "IOrbitalMechanics.hpp"
#include "OrbitalMechanics.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

Application::Application(std::shared_ptr<IBodyDatabase>            db,
                         std::shared_ptr<IConstellationCalculator> calculator,
                         std::shared_ptr<IReportFormatter>         formatter,
                         std::shared_ptr<ITimeFormatter>           time_formatter)
    : db_(std::move(db))
    , calculator_(std::move(calculator))
    , formatter_(std::move(formatter))
    , time_formatter_(std::move(time_formatter)) {}

// ── Internal helpers ──────────────────────────────────────────────────────────

static std::string read_line(const char* prompt) {
    std::printf("%s", prompt);
    std::fflush(stdout);
    std::string line;
    std::getline(std::cin, line);
    // trim trailing whitespace
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back())))
        line.pop_back();
    return line;
}

static bool is_digits(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(),
        [](unsigned char c){ return std::isdigit(c); });
}

// ── Body selection ────────────────────────────────────────────────────────────

static CelestialBody select_body(const std::shared_ptr<IBodyDatabase>& db) {
    auto all = db->all();
    std::sort(all.begin(), all.end(),
        [](const CelestialBody& a, const CelestialBody& b){ return a.name < b.name; });

    std::printf("\nAvailable bodies:\n");
    for (std::size_t i = 0; i < all.size(); ++i) {
        const auto& b = all[i];
        char atm_buf[32]  = "";
        char par_buf[64]  = "";
        if (b.atm > 0.0)
            std::snprintf(atm_buf, sizeof(atm_buf), "  atm up%g km",
                          b.atm / 1000.0);
        if (b.parent.has_value())
            std::snprintf(par_buf, sizeof(par_buf), "orbits %s", b.parent->c_str());
        else
            std::snprintf(par_buf, sizeof(par_buf), "the star");
        std::printf("  %2zu. %-10s  R=%.0f km%s  (%s)\n",
                    i + 1, b.name.c_str(), b.radius / 1000.0, atm_buf, par_buf);
    }
    std::putchar('\n');

    while (true) {
        std::string raw = read_line("Select body (number or name): ");
        if (is_digits(raw)) {
            std::size_t idx = static_cast<std::size_t>(std::stoul(raw)) - 1;
            if (idx < all.size()) return all[idx];
        }
        auto found = db->find(raw);
        if (found.has_value()) return *found;
        std::printf("  Invalid selection -- enter a number or exact name.\n");
    }
}

// ── Altitude menu ─────────────────────────────────────────────────────────────

static double ask_altitude(const CelestialBody& body,
                            const std::shared_ptr<IBodyDatabase>& db,
                            const std::shared_ptr<IOrbitalMechanics>& mech) {
    const double atm_km   = body.atm / 1000.0;
    const auto   h_sync   = mech->sync_orbit_altitude(body);
    const auto   h_soi_max= mech->soi_max_altitude(body);
    const auto   kids     = db->children_of(body.name);
    const char*  cword    = body.parent.has_value() ? "moon" : "planet";

    // Build menu
    struct Entry { std::string label; std::string action; };
    std::vector<Entry> menu;

    {
        char sync_tag[128];
        if (!h_sync.has_value()) {
            std::snprintf(sync_tag, sizeof(sync_tag), "below surface / unavailable");
        } else if (h_soi_max.has_value() && *h_sync > *h_soi_max) {
            std::snprintf(sync_tag, sizeof(sync_tag),
                          "%.2f km  (exceeds SOI limit %.2f km)",
                          *h_sync / 1000.0, *h_soi_max / 1000.0);
        } else {
            std::snprintf(sync_tag, sizeof(sync_tag), "%.2f km", *h_sync / 1000.0);
        }
        menu.push_back({"Synchronous orbit  ->  " + std::string(sync_tag), "sync"});
    }

    if (!kids.empty()) {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "At a %s's orbital distance", cword);
        menu.push_back({buf, "child_single"});
        if (kids.size() >= 2) {
            std::snprintf(buf, sizeof(buf), "Midpoint between two %ss", cword);
            menu.push_back({buf, "child_mid"});
        }
    }
    menu.push_back({"Custom altitude", "custom"});

    // Print menu
    if (body.atm > 0.0)
        std::printf("\nTarget circular orbit altitude above %s  (atmosphere ends at %.0f km)\n",
                    body.name.c_str(), atm_km);
    else
        std::printf("\nTarget circular orbit altitude above %s\n", body.name.c_str());

    for (std::size_t i = 0; i < menu.size(); ++i)
        std::printf("  %zu. %s\n", i + 1, menu[i].label.c_str());
    std::putchar('\n');

    while (true) {
        char prompt[32];
        std::snprintf(prompt, sizeof(prompt), "  Option [1-%zu]: ", menu.size());
        std::string choice = read_line(prompt);
        if (!is_digits(choice)) { std::printf("  Enter a number.\n"); continue; }
        std::size_t idx = static_cast<std::size_t>(std::stoul(choice)) - 1;
        if (idx >= menu.size()) { std::printf("  Enter a number between 1 and %zu.\n", menu.size()); continue; }

        const std::string& action = menu[idx].action;

        // ── Sync orbit ────────────────────────────────────────────
        if (action == "sync") {
            if (!h_sync.has_value()) {
                std::printf("  Synchronous orbit is below the surface. Choose another option.\n");
                continue;
            }
            if (h_soi_max.has_value() && *h_sync > *h_soi_max) {
                std::printf("\n  Synchronous orbit altitude (%.2f km) exceeds %s's SOI limit (%.2f km).\n"
                            "     A satellite at this altitude would escape %s's gravity.\n"
                            "     Please choose another option with an altitude below %.2f km.\n\n",
                            *h_sync / 1000.0, body.name.c_str(), *h_soi_max / 1000.0,
                            body.name.c_str(), *h_soi_max / 1000.0);
                continue;
            }
            std::printf("  Using synchronous orbit altitude: %.2f km\n", *h_sync / 1000.0);
            return *h_sync;
        }

        // ── At a child's orbital distance ─────────────────────────
        if (action == "child_single") {
            std::printf("\n  Select %s:\n", cword);
            for (std::size_t i = 0; i < kids.size(); ++i) {
                double h_child = (*kids[i].orbital_radius - body.radius) / 1000.0;
                std::printf("    %zu. %-10s  orbital altitude: %.2f km\n",
                            i + 1, kids[i].name.c_str(), h_child);
            }
            while (true) {
                char p[32]; std::snprintf(p, sizeof(p), "    %s [number]: ",
                                         (std::string(1, static_cast<char>(std::toupper(
                                             static_cast<unsigned char>(cword[0])))) +
                                          std::string(cword + 1)).c_str());
                std::string raw = read_line(p);
                if (is_digits(raw)) {
                    std::size_t i = static_cast<std::size_t>(std::stoul(raw)) - 1;
                    if (i < kids.size()) {
                        double h = *kids[i].orbital_radius - body.radius;
                        std::printf("  Using %s's orbital altitude: %.2f km\n",
                                    kids[i].name.c_str(), h / 1000.0);
                        return h;
                    }
                }
                std::printf("    Enter a number between 1 and %zu.\n", kids.size());
            }
        }

        // ── Midpoint between two children ─────────────────────────
        if (action == "child_mid") {
            std::printf("\n  Select two %ss to compute the midpoint altitude:\n", cword);
            for (std::size_t i = 0; i < kids.size(); ++i) {
                double h_child = (*kids[i].orbital_radius - body.radius) / 1000.0;
                std::printf("    %zu. %-10s  orbital altitude: %.2f km\n",
                            i + 1, kids[i].name.c_str(), h_child);
            }
            std::size_t idx_a = 0;
            while (true) {
                char p[32]; std::snprintf(p, sizeof(p), "    First %s [number]: ", cword);
                std::string raw = read_line(p);
                if (is_digits(raw)) {
                    std::size_t i = static_cast<std::size_t>(std::stoul(raw)) - 1;
                    if (i < kids.size()) { idx_a = i; break; }
                }
                std::printf("    Enter a number between 1 and %zu.\n", kids.size());
            }
            std::size_t idx_b = 0;
            while (true) {
                char p[32]; std::snprintf(p, sizeof(p), "    Second %s [number]: ", cword);
                std::string raw = read_line(p);
                if (is_digits(raw)) {
                    std::size_t i = static_cast<std::size_t>(std::stoul(raw)) - 1;
                    if (i < kids.size()) {
                        if (i == idx_a) {
                            std::printf("    Choose a different %s than the first.\n", cword);
                            continue;
                        }
                        idx_b = i; break;
                    }
                }
                std::printf("    Enter a number between 1 and %zu.\n", kids.size());
            }
            double r_mid = (*kids[idx_a].orbital_radius + *kids[idx_b].orbital_radius) / 2.0;
            double h = r_mid - body.radius;
            std::printf("  Midpoint between %s and %s: %.2f km\n",
                        kids[idx_a].name.c_str(), kids[idx_b].name.c_str(), h / 1000.0);
            return h;
        }

        // ── Custom altitude ───────────────────────────────────────
        while (true) {
            std::string raw = read_line("  Altitude [km]: ");
            double h = 0.0;
            try { h = std::stod(raw) * 1000.0; }
            catch (...) { std::printf("  Please enter a number.\n"); continue; }
            if (h <= 0.0) { std::printf("  Altitude must be positive.\n"); continue; }
            if (body.atm > 0.0 && h < body.atm) {
                std::printf("  That altitude is inside the atmosphere (%.0f km). Try higher.\n", atm_km);
                continue;
            }
            if (h_soi_max.has_value() && h > *h_soi_max) {
                std::printf("  Altitude (%.2f km) exceeds %s's SOI limit (%.2f km). Enter a lower altitude.\n",
                            h / 1000.0, body.name.c_str(), *h_soi_max / 1000.0);
                continue;
            }
            return h;
        }
    }
}

// ── run() ─────────────────────────────────────────────────────────────────────

void Application::run() {
    // Provide OrbitalMechanics to ask_altitude (needed for sync/SOI queries)
    auto mech = std::make_shared<OrbitalMechanics>();

    std::printf("\n");
    for (int i = 0; i < 62; ++i) std::putchar('=');
    std::printf("\n  KSP -- 3-Satellite Relay Constellation Calculator\n");
    std::printf("  Method: elliptical parking orbit, T_e = (2/3) T_c\n");
    for (int i = 0; i < 62; ++i) std::putchar('=');
    std::putchar('\n');

    while (true) {
        CelestialBody body = select_body(db_);
        double h_target    = ask_altitude(body, db_, mech);
        OrbitalResult res  = calculator_->calculate(body, h_target);
        formatter_->print(res);

        std::string again = read_line("\nCalculate for another body/altitude? [y/N]: ");
        // lowercase
        for (char& c : again) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (again != "y" && again != "yes") break;
    }

    std::printf("\nGood luck with the mission!\n\n");
}
