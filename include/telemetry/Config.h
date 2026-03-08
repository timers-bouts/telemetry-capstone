#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace telemetry {

    enum class CliCommand {
        Generate, // Simulate N frames, print to console (no file output)
        Record,   // Simulate frames and record to binary file
        Replay,   // Read and validate a binary telemetry file
        Export,   // Read binary file and export to CSV or JSON
        Help,
        Unknown,
    };

    struct Config {
        CliCommand  command     = CliCommand::Help;

        // File paths
        std::string input_file  = "telemetry.bin";
        std::string output_file = "telemetry.bin";
        std::string log_file    = "capstone.log";

        // Verbosity
        bool        verbose     = false;

        // Simulator options
        std::size_t frame_count = 100;
        uint64_t    step_ms     = 100;
        uint32_t    seed        = 42;

        // Export options
        std::string export_format = "csv"; // "csv" or "json"

        // Parse argc/argv.
        // Returns std::nullopt if args are invalid or --help was requested.
        static std::optional<Config> parse(int argc, char* argv[]);

        // Print usage to stdout.
        static void print_help(const char* program_name);
    };

} // namespace telemetry
