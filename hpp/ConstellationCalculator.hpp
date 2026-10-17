#pragma once
#include "IConstellationCalculator.hpp"
#include "IOrbitalMechanics.hpp"
#include <memory>

// (DIP) Depends on IOrbitalMechanics, not the concrete class.
class ConstellationCalculator final : public IConstellationCalculator {
public:
    explicit ConstellationCalculator(std::shared_ptr<IOrbitalMechanics> mechanics);

    [[nodiscard]] OrbitalResult
    calculate(const CelestialBody& body, double h_target) const override;

private:
    std::shared_ptr<IOrbitalMechanics> mechanics_;
};
