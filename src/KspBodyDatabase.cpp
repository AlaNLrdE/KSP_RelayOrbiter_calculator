#include "KspBodyDatabase.hpp"

// STUB — static table intentionally empty until implementation step.
const std::vector<CelestialBody> KspBodyDatabase::bodies_ = {};

std::optional<CelestialBody> KspBodyDatabase::find(std::string_view) const {
    return std::nullopt;
}

std::vector<CelestialBody> KspBodyDatabase::all() const {
    return {};
}

std::vector<CelestialBody> KspBodyDatabase::children_of(std::string_view) const {
    return {};
}
