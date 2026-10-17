#include "ConstellationCalculator.hpp"

ConstellationCalculator::ConstellationCalculator(
    std::shared_ptr<IOrbitalMechanics> mechanics)
    : mechanics_(std::move(mechanics)) {}

// STUB — returns a default-constructed OrbitalResult (all zeros / false).
OrbitalResult ConstellationCalculator::calculate(const CelestialBody&, double) const {
    return OrbitalResult{};
}
