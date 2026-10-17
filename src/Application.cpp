#include "Application.hpp"

Application::Application(std::shared_ptr<IBodyDatabase>            db,
                         std::shared_ptr<IConstellationCalculator> calculator,
                         std::shared_ptr<IReportFormatter>         formatter,
                         std::shared_ptr<ITimeFormatter>           time_formatter)
    : db_(std::move(db))
    , calculator_(std::move(calculator))
    , formatter_(std::move(formatter))
    , time_formatter_(std::move(time_formatter)) {}

// STUB — does nothing until implementation step.
void Application::run() {}
