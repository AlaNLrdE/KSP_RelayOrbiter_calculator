#pragma once
#include "IBodyDatabase.hpp"
#include "IConstellationCalculator.hpp"
#include "IReportFormatter.hpp"
#include "ITimeFormatter.hpp"
#include <memory>

// Top-level orchestrator. All dependencies injected — never instantiates
// concrete classes itself (DIP + OCP: swap any dependency without touching this).
class Application {
public:
    Application(std::shared_ptr<IBodyDatabase>            db,
                std::shared_ptr<IConstellationCalculator> calculator,
                std::shared_ptr<IReportFormatter>         formatter,
                std::shared_ptr<ITimeFormatter>           time_formatter);

    void run();

private:
    std::shared_ptr<IBodyDatabase>            db_;
    std::shared_ptr<IConstellationCalculator> calculator_;
    std::shared_ptr<IReportFormatter>         formatter_;
    std::shared_ptr<ITimeFormatter>           time_formatter_;
};
