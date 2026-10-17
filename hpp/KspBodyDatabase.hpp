#pragma once
#include "IBodyDatabase.hpp"

// Concrete implementation hardcoding all 17 stock KSP bodies.
// Adding a modded body only requires editing the static table in the .cpp.
class KspBodyDatabase final : public IBodyDatabase {
public:
    [[nodiscard]] std::optional<CelestialBody>
    find(std::string_view name) const override;

    [[nodiscard]] std::vector<CelestialBody>
    all() const override;

    [[nodiscard]] std::vector<CelestialBody>
    children_of(std::string_view parent_name) const override;

private:
    static const std::vector<CelestialBody> bodies_;
};
