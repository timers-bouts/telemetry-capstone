#include <iostream>

#include "telemetry/Logger.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetrySimulator.h"

// recorder_demo: Runs a short simulation and writes the result to a binary file.
int main() {
    
    auto logger = telemetry::Logger("recorder_demo.log",
                                    telemetry::LogLevel::Info,
                                    false);
    
    telemetry::TelemetrySimulator::Config config;
    config.packet_count = 50;
    config.step_ms = 100;
    config.seed = 42;

    const std::string output_path = "data/recorder_demo.bin";

    auto tr = telemetry::TelemetryRecorder(output_path,
                        logger,
                        telemetry::TelemetryRecorder::OpenMode::Truncate);

    auto ts = telemetry::TelemetrySimulator(config,
                                            tr,
                                            logger,
                                            false);
    
    ts.run();

    std::cout << "Recorded "
              << config.packet_count
              << " packets to "
              << output_path
              << "\n";
    
    return 0;
}