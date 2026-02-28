#include <iostream>

#include "telemetry/Config.h"
#include "Commands.h"

int main(int argc, char* argv[]) {
    auto cfg = telemetry::Config::parse(argc, argv);
    if (!cfg) {
        return 1;
    }

    switch (cfg->command) {
        case telemetry::CliCommand::Generate: return telemetry::cli::run_generate(*cfg);
        case telemetry::CliCommand::Record:   return telemetry::cli::run_record(*cfg);
        case telemetry::CliCommand::Replay:   return telemetry::cli::run_replay(*cfg);
        case telemetry::CliCommand::Export:   return telemetry::cli::run_export(*cfg);
        case telemetry::CliCommand::Help:
        case telemetry::CliCommand::Unknown:
        default:
            telemetry::Config::print_help(argv[0]);
            return 0;
    }
}