#pragma once

#include <cstdint>
#include <random>
#include <vector>
#include <span>

#include "telemetry/TelemetryFrame.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/Logger.h"

namespace telemetry {

    class TelemetrySimulator {
        public:
            // Simulation configuration
            struct Config {
                uint64_t start_time_ms = 0;      // simulation start timestamp (ms)
                uint64_t step_ms       = 100;     // time per frame in ms
                size_t   packet_count  = 20;      // number of packets to generate
                uint32_t seed          = 42;      // RNG seed for deterministic output
                uint64_t end_time_ms   = start_time_ms + packet_count * step_ms;
            };

            // Constructor: takes configuration, a recorder instance, and a logger
            TelemetrySimulator(const Config& config,
                               TelemetryRecorder& recorder,
                               Logger& logger,
                               bool saveFrames = false);

            ~TelemetrySimulator() = default;

            // Run the simulation: generates frames, serializes, and records them
            void run();

            // Returns the stored frames (only populated if saveFrames = true)
            std::span<const TelemetryFrame> get_frames() const;

        private:
            // Generate a single frame based on current simulator state
            TelemetryFrame generate_frame();

            // Advance internal physics/sensor state by one step
            void advance_state();

            Config config_;
            TelemetryRecorder& recorder_;
            Logger& logger_;

            // Random number generator
            std::mt19937 rng_;

            // Internal simulation state
            uint64_t current_time_ms_;
            float position_x_;
            float position_y_;
            float velocity_mps_;
            float temperature_c_;
            float base_temperature_;
            float temperature_wave_angle_;
            float period_seconds_;
            float angular_velocity_;
            float temperature_wave_amplitude_;
            float voltage_v_;
            float voltage_decay_;

            // Noise distributions
            std::normal_distribution<float> noise_position_;
            std::normal_distribution<float> noise_velocity_;
            std::normal_distribution<float> noise_temperature_;
            std::normal_distribution<float> noise_voltage_;

            std::vector<TelemetryFrame> frames_;
            bool save_frames_;
    };

} // namespace telemetry
