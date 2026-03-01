#include "Commands.h"

#include "telemetry/Logger.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetryReader.h"
#include "telemetry/TelemetrySimulator.h"
#include "telemetry/PacketReader.h"
#include "telemetry/TelemetryFrame.h"
#include "telemetry/Exporter.h"

#include <iostream>
#include <vector>
#include <span>
#include <iomanip>

namespace telemetry::cli {

    // Helper method to print data frame Telemetry Frames in readable table
    static void print_frame_table( std::span<const telemetry::TelemetryFrame> frames) {
        // Print header
        std::cout << std::setw(10) << "Time(ms)"
                << std::setw(12) << "Temp(C)"
                << std::setw(10) << "Volt(V)"
                << std::setw(12) << "PosX(m)"
                << std::setw(12) << "PosY(m)"
                << std::setw(12) << "Vel(m/s)"
                << std::setw(10) << "Flags"
                << "\n";

        std::cout << std::string(78, '-') << "\n";

        // Print each frame
        std::cout << std::fixed << std::setprecision(2);
        for (const auto& f : frames) {
            std::cout << std::setw(10) << f.timestamp_ms
                    << std::setw(12) << f.temperature_c
                    << std::setw(10) << f.voltage_v
                    << std::setw(12) << f.position_x
                    << std::setw(12) << f.position_y
                    << std::setw(12) << f.velocity_mps
                    << std::setw(10) << f.status_flags
                    << "\n";
        }
    }

    int run_generate(const Config& cfg) {
        
        auto logger = telemetry::Logger(
            cfg.log_file,
            telemetry::LogLevel::Info,
            cfg.verbose
        );
        
        telemetry::TelemetrySimulator::Config config;
        config.packet_count = cfg.frame_count;
        config.step_ms = cfg.step_ms;
        config.seed = cfg.seed;

        auto recorder = telemetry::TelemetryRecorder(
            "generate.bin",
            logger,
            telemetry::TelemetryRecorder::OpenMode::Truncate
        );

        auto simulator = telemetry::TelemetrySimulator(
            config,
            recorder,
            logger,
            true
        );

        simulator.run();

        print_frame_table(simulator.get_frames());

        return 0;
    }

    int run_record(const Config& cfg) {
        
        auto logger = telemetry::Logger(
            cfg.log_file,
            telemetry::LogLevel::Info,
            cfg.verbose
        );
        
        telemetry::TelemetrySimulator::Config config;
        config.packet_count = cfg.frame_count;
        config.step_ms = cfg.step_ms;
        config.seed = cfg.seed;

        auto recorder = telemetry::TelemetryRecorder(
            cfg.output_file,
            logger,
            telemetry::TelemetryRecorder::OpenMode::Truncate
        );

        auto simulator = telemetry::TelemetrySimulator(
            config,
            recorder,
            logger,
            false
        );

        simulator.run();

        std::cout << "Recorded "
                << cfg.frame_count
                << " frames to "
                << cfg.output_file
                << "\n";

        return 0;
    }

    int run_replay(const Config& cfg) {
        
        auto logger = telemetry::Logger(
            cfg.log_file,
            telemetry::LogLevel::Info,
            cfg.verbose
        );

        try {
            auto tr = telemetry::TelemetryReader::open(cfg.input_file, logger);

            std::vector<std::uint8_t> packet;
            std::vector<telemetry::TelemetryFrame> frames;
            while (tr.read_next(packet)) {
                auto pr = telemetry::PacketReader(packet);
                telemetry::TelemetryFrame frame;
                frame.deserialize(pr);
                frames.push_back(frame);
            }

            print_frame_table(std::span(frames));
            
            std::cout << "Read "
                    << frames.size()
                    << " frames from "
                    << cfg.input_file
                    << "\n";

        } catch (const telemetry::ParseError& e) {
            std::cerr << "Error reading "
                    << cfg.input_file
                    << ": "
                    << e.what()
                    << "\n";
            return 1;
        }

        return 0;
    }

    int run_export(const Config& cfg) {
        
        auto logger = telemetry::Logger(
            cfg.log_file,
            telemetry::LogLevel::Info,
            cfg.verbose
        );

        try {
            auto tr = telemetry::TelemetryReader::open(cfg.input_file, logger);

            std::vector<std::uint8_t> packet;
            std::vector<telemetry::TelemetryFrame> frames;
            while (tr.read_next(packet)) {
                auto pr = telemetry::PacketReader(packet);
                telemetry::TelemetryFrame frame;
                frame.deserialize(pr);
                frames.push_back(frame);
            }

            auto exporter = telemetry::Exporter(logger);
            telemetry::Exporter::Format format;

            if (cfg.export_format == "csv") {
                format = telemetry::Exporter::Format::CSV;
            } else {
                format = telemetry::Exporter::Format::JSON;
            }

            if (!exporter.export_to(frames, cfg.output_file,format)) {
                std::cerr << "Error exporting frames to "
                          << cfg.output_file
                          << "\n";
                return 1;
            }

            std::cout << "Exported "
                      << frames.size()
                      << " frames to "
                      << cfg.output_file
                      << "\n";

        } catch (const telemetry::ParseError& e) {
            std::cerr << "Error reading "
                    << cfg.input_file
                    << ": "
                    << e.what()
                    << "\n";
            return 1;
        }

        return 0;
    }

} // namespace telemetry::cli