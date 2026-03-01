#include <iostream>
#include <vector>
#include <span>
#include <iomanip>

#include "telemetry/Logger.h"
#include "telemetry/TelemetryReader.h"
#include "telemetry/PacketReader.h"
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

// reader_demo: Reads a binary telemetry file and prints each frame.
int main() {
    
    auto logger = telemetry::Logger("reader_demo.log",
                                    telemetry::LogLevel::Info,
                                    false);
    std::string input_path = "data/recorder_demo.bin";

    try {
        auto tr = telemetry::TelemetryReader::open(input_path, logger);

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
                  << input_path
                  << "\n";

    } catch (const telemetry::ParseError& e) {
        std::cerr << "Error reading "
                  << input_path
                  << ": "
                  << e.what();
        return 1;
    }
    
    return 0;
}