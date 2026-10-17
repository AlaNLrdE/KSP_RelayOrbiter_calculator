#pragma once
#include "CelestialBody.hpp"
#include <optional>
#include <string_view>
#include <vector>

// (ISP) Narrow read-only interface for accessing the body catalogue.
class IBodyDatabase {
public:
    virtual ~IBodyDatabase() = default;

    [[nodiscard]] virtual std::optional<CelestialBody>
    find(std::string_view name) const = 0;

    [[nodiscard]] virtual std::vector<CelestialBody>
    all() const = 0;

    // Returns direct children sorted by orbital_radius ascending.
    [[nodiscard]] virtual std::vector<CelestialBody>
    children_of(std::string_view parent_name) const = 0;
};
