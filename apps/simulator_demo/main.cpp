#include <iostream>
#include <iomanip>
#include <span>

#include "telemetry/Logger.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetrySimulator.h"
#include "telemetry/TelemetryFrame.h"

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

// simulator_demo: Generates telemetry frames and prints a tabular view to stdout.
int main() {
    
    telemetry::Logger logger("simulator_demo.log",
                            telemetry::LogLevel::Info,
                            false);
    
    telemetry::TelemetrySimulator::Config config;
    config.packet_count = 50;
    config.step_ms = 100;
    config.seed = 42;

    auto tr = telemetry::TelemetryRecorder("data/simulator_demo.bin",
                        logger,
                        telemetry::TelemetryRecorder::OpenMode::Truncate);

    auto ts = telemetry::TelemetrySimulator(config,
                                            tr,
                                            logger,
                                            true);
                                                 
    ts.run();
    print_frame_table(ts.get_frames());

    return 0;
}