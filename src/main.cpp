#include "Application.hpp"
#include "ConstellationCalculator.hpp"
#include "ConsoleReportFormatter.hpp"
#include "KerbinTimeFormatter.hpp"
#include "KspBodyDatabase.hpp"
#include "OrbitalMechanics.hpp"
#include <memory>

int main() {
    auto db        = std::make_shared<KspBodyDatabase>();
    auto mechanics = std::make_shared<OrbitalMechanics>();
    auto calculator= std::make_shared<ConstellationCalculator>(mechanics);
    auto time_fmt  = std::make_shared<KerbinTimeFormatter>();
    auto formatter = std::make_shared<ConsoleReportFormatter>(time_fmt);

    Application app(db, calculator, formatter, time_fmt);
    app.run();
    return 0;
}
