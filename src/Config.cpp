#include "telemetry/Config.h"

#include <iostream>
#include <string>
#include <cstring>

namespace telemetry {

std::optional<Config> Config::parse(int argc, char* argv[]) {
    // TODO: Implement argument parsing.
    //
    // Suggested CLI design:
    //   capstone_cli generate [--frames N] [--step-ms N] [--seed N] [--log FILE] [-v]
    //   capstone_cli record   [--frames N] [--step-ms N] [--seed N] --output FILE [-v]
    //   capstone_cli replay   --input FILE [-v]
    //   capstone_cli export   --input FILE --output FILE --format csv|json [-v]
    //   capstone_cli help
    //
    // Steps:
    //   1. Check argc >= 2, else print_help and return nullopt
    //   2. Parse argv[1] as the command string -> set cfg.command
    //   3. Loop remaining argv, match --flag value pairs
    //   4. Validate required args per command (e.g. replay needs --input)
    //   5. Return cfg on success, nullopt on error

    if (argc < 2) {
        print_help(argv[0]);
        return std::nullopt;
    }

    Config cfg;

    // Placeholder: just map the command string
    std::string cmd = argv[1];
    if      (cmd == "generate") cfg.command = CliCommand::Generate;
    else if (cmd == "record")   cfg.command = CliCommand::Record;
    else if (cmd == "replay")   cfg.command = CliCommand::Replay;
    else if (cmd == "export")   cfg.command = CliCommand::Export;
    else if (cmd == "help")     cfg.command = CliCommand::Help;
    else {
        std::cerr << "Unknown command: " << cmd << "\n";
        print_help(argv[0]);
        return std::nullopt;
    }

    // TODO: parse remaining flags (--frames, --output, --input, --format, -v, etc.)

    return cfg;
}

void Config::print_help(const char* program_name) {
    // TODO: Expand with full flag documentation for each command.
    std::cout
        << "Usage: " << program_name << " <command> [options]\n\n"
        << "Commands:\n"
        << "  generate   Simulate frames and print to console\n"
        << "  record     Simulate frames and record to binary file\n"
        << "  replay     Read and validate a binary telemetry file\n"
        << "  export     Export binary file to CSV or JSON\n"
        << "  help       Show this message\n\n"
        << "Options (TODO: per-command flags)\n";
}

} // namespace telemetry