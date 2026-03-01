#include "telemetry/Config.h"

#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include <span>

namespace telemetry {

    // Helper for parsing numeric arguments
    static std::optional<unsigned long> parse_ulong(const char* s, const std::string& flag) {
        try {
            return std::stoul(s);
        } catch (const std::exception&) {
            std::cerr << flag << " requires a valid number, got: " << s << "\n";
            return std::nullopt;
        }
    }

    // Helper for printing command flag options in print_help()
    static void print_options(std::span<const std::string> flags,
                        std::span<const std::string> explanations,
                        std::span<const std::string> defaults) {
    
        std::cout << std::left;
        for (unsigned long i = 0; i < flags.size(); i++) {
            std::cout << std::setw(18) << flags[i]
                      << std::setw(30) << explanations[i]
                      << defaults[i]
                      << "\n";
        }

    }

    std::optional<Config> Config::parse(int argc, char* argv[]) {

        if (argc < 2) {
            print_help(argv[0]);
            return std::nullopt;
        }

        Config cfg;

        // Map the command string
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

        std::map<std::string, bool> arg_passed = {
            {"--input", false},
            {"--output", false},
            {"--format", false}
        };

        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];

            if (arg == "--frames") {
                if (i + 1 >= argc) {
                    std::cerr << "--frames requires a value\n";
                    return std::nullopt;
                }
                auto val = parse_ulong(argv[++i], "--frames");
                if (!val) return std::nullopt;
                cfg.frame_count = static_cast<std::size_t>(*val);
            } else if (arg == "--output") {
                if (i + 1 >= argc) {
                    std::cerr << "--output requires a value\n";
                    return std::nullopt;
                }
                cfg.output_file = argv[++i];
                arg_passed[arg] = true;
            } else if (arg == "--input") {
                if (i + 1 >= argc) {
                    std::cerr << "--input requires a value\n";
                    return std::nullopt;
                }
                cfg.input_file = argv[++i];
                arg_passed[arg] = true;
            } else if (arg == "-v") {
                cfg.verbose = true;
            } else if (arg == "--format") {
                if (i + 1 >= argc) {
                    std::cerr << "--format requires a value\n";
                    return std::nullopt;
                }
                cfg.export_format = argv[++i];
                arg_passed[arg] = true;
            } else if (arg == "--seed") {
                if (i + 1 >= argc) {
                    std::cerr << "--seed requires a value\n";
                    return std::nullopt;
                }
                auto val = parse_ulong(argv[++i], "--seed");
                if (!val) return std::nullopt;
                cfg.seed = static_cast<std::uint32_t>(*val);
            } else if (arg == "--step-ms") {
                if (i + 1 >= argc) {
                    std::cerr << "--step-ms requires a value\n";
                    return std::nullopt;
                }
                auto val = parse_ulong(argv[++i], "--step-ms");
                if (!val) return std::nullopt;
                cfg.step_ms = static_cast<std::uint64_t>(*val);
            } else if (arg == "--log") {
                if (i + 1 >= argc) {
                    std::cerr << "--log requires a value\n";
                    return std::nullopt;
                }
                cfg.log_file = argv[++i];
            } else {
                std::cerr << "Unknown flag: " << arg << "\n";
                return std::nullopt;
            }

        }

        // Validate all command line arguments
        if (cfg.command == CliCommand::Record && !arg_passed["--output"]) {
            std::cerr << "Record command requires an output file\n";
            return std::nullopt;
        } else if (cfg.command == CliCommand::Replay && !arg_passed["--input"]) {
            std::cerr << "Replay command requires an input file\n";
            return std::nullopt;
        } else if (cfg.command == CliCommand::Export && (
                    !arg_passed["--input"] ||
                    !arg_passed["--output"] ||
                    !arg_passed["--format"])) {
            std::cerr << "Export command requires output and inputs file and format type\n";
            return std::nullopt;
        }

        if (arg_passed["--format"] && cfg.export_format != "csv" && cfg.export_format != "json") {
            std::cerr << "Unknown format: " << cfg.export_format << " (use csv or json)\n";
            return std::nullopt;
        }

        return cfg;
    }

    void Config::print_help(const char* program_name) {

        std::vector<std::string> generate_flags = {
            "  --frames N",
            "  --step-ms N",
            "  --seed N",
            "  --log FILE",
            "  -v"
        };

        std::vector<std::string> generate_explanations = {
            "Number of frames to simulate",
            "Milliseconds between frames",
            "RNG seed",
            "Log output file",
            "Verbose logging"
        };

        std::vector<std::string> generate_defaults = {
            "(default: 100)",
            "(default: 100)",
            "(default: 42)",
            "(default: capstone.log)",
            ""
        };

        std::vector<std::string> record_flags = {
            "  --output FILE",
            "  --frames N",
            "  --step-ms N",
            "  --seed N",
            "  --log FILE",
            "  -v"
        };

        std::vector<std::string> record_explanations = {
            "Output binary file",
            "Number of frames to simulate",
            "Milliseconds between frames",
            "RNG seed",
            "Log output file",
            "Verbose logging",
        };

        std::vector<std::string> record_defaults = {
            "(required)",
            "(default: 100)",
            "(default: 100)",
            "(default: 42)",
            "(default: capstone.log)",
            ""
        };

        std::vector<std::string> replay_flags = {
            "  --input FILE",
            "  --log FILE",
            "  -v"
        };

        std::vector<std::string> replay_explanations = {
            "Input binary file",
            "Log output file",
            "Verbose logging"
        };

        std::vector<std::string> replay_defaults = {
            "(required)",
            "(default: capstone.log)",
            ""
        };

        std::vector<std::string> export_flags = {
            "  --input FILE",
            "  --output FILE",
            "  --format FMT ",
            "  --log FILE",
            "  -v"
        };

        std::vector<std::string> export_explanations = {
            "Input binary file",
            "Output binary file",
            "Output format: csv or json",
            "Log output file",
            "Verbose logging"
        };

        std::vector<std::string> export_defaults = {
            "(required)",
            "(required)",
            "(required)",
            "(default: capstone.log)",
            ""
        };

        std::cout
            << "Usage: " << program_name << " <command> [options]\n\n"
            << "Commands:\n"
            << "  generate   Simulate frames and print to console\n"
            << "  record     Simulate frames and record to binary file\n"
            << "  replay     Read and validate a binary telemetry file\n"
            << "  export     Export binary file to CSV or JSON\n"
            << "  help       Show this message\n\n"
            << "generate options:\n";

        print_options(std::span<const std::string>(generate_flags),
                        std::span<const std::string>(generate_explanations),
                        std::span<const std::string>(generate_defaults));
        
        std::cout << "\n"
                  << "record options:\n";

        print_options(std::span<const std::string>(record_flags),
                        std::span<const std::string>(record_explanations),
                        std::span<const std::string>(record_defaults));
        
        std::cout << "\n"
                  << "replay options:\n";

        print_options(std::span<const std::string>(replay_flags),
                        std::span<const std::string>(replay_explanations),
                        std::span<const std::string>(replay_defaults));
        
        std::cout << "\n"
                  << "export options:\n";
        
        print_options(std::span<const std::string>(export_flags),
                        std::span<const std::string>(export_explanations),
                        std::span<const std::string>(export_defaults));
        
        std::cout << "\n";

    }

} // namespace telemetry