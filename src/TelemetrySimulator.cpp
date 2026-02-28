#include <cmath>

#include "telemetry/PacketWriter.h"
#include "telemetry/PacketReader.h"
#include "telemetry/TelemetrySimulator.h"

namespace telemetry {

    TelemetrySimulator::TelemetrySimulator(const Config& config, telemetry::TelemetryRecorder& recorder,
                                            telemetry::Logger& logger, bool saveFrames)
        : config_(config),
          recorder_(recorder),
          logger_(logger),
          rng_(config.seed),
          current_time_ms_(config_.start_time_ms),
          position_x_(0.0f),
          position_y_(0.0f),
          velocity_mps_(0.1f),
          temperature_c_(20.0f), // Arbitrary default value
          base_temperature_(20.0f),
          temperature_wave_angle_(0.0f),
          period_seconds_(10.0f),
          angular_velocity_((2 * M_PI) / period_seconds_),
          temperature_wave_amplitude_(0.1f),
          voltage_v_(12.0f),     // Arbitrary default value
          voltage_decay_(0.04f),
          noise_position_(0.0f, 0.1f),
          noise_velocity_(0.0f, 0.05f),
          noise_temperature_(0.0f, 0.5f),
          noise_voltage_(0.0f, 0.02f),
          save_frames_(saveFrames)
          {}

        void TelemetrySimulator::advance_state() {
            float dt = config_.step_ms / 1000.0f;

            position_x_ += velocity_mps_ * dt;
            position_y_ += velocity_mps_ * dt;
            temperature_c_ = base_temperature_ + temperature_wave_amplitude_ * sin(temperature_wave_angle_);
            voltage_v_ = std::max(0.0f, voltage_v_ - voltage_decay_ * dt);
            temperature_wave_angle_ += angular_velocity_ * dt;
            if (temperature_wave_angle_ > 2 * M_PI) {
                temperature_wave_angle_ -=  2 * M_PI;
            }
            current_time_ms_ += config_.step_ms;

        }

        TelemetryFrame TelemetrySimulator::generate_frame() {
            TelemetryFrame frame;
            frame.timestamp_ms = current_time_ms_;

            // Add random noise values to simulate sensor noise (all values set determined by seed for testing/debugging)
            frame.position_x = position_x_ + noise_position_(rng_);
            frame.position_y = position_y_ + noise_position_(rng_);
            frame.velocity_mps = velocity_mps_ + noise_velocity_(rng_);
            frame.temperature_c = temperature_c_ + noise_temperature_(rng_);
            frame.voltage_v = voltage_v_ + noise_voltage_(rng_);

            return frame;
        }

        std::span<const TelemetryFrame> TelemetrySimulator::get_frames() const {
            return std::span<const TelemetryFrame>(frames_);
        }

        void TelemetrySimulator::run() {

            logger_.info("Simulation started");
            logger_.info(
                "Config: start=" + std::to_string(config_.start_time_ms) +
                "ms, end=" + std::to_string(config_.end_time_ms) +
                "ms, step=" + std::to_string(config_.step_ms) +
                "ms, seed=" + std::to_string(config_.seed)
            );

            frames_.clear();  // ensure clean run

            logger_.info("Initial state: "
                "position=(" + std::to_string(position_x_) + ", "
                                + std::to_string(position_y_) + "), "
                "velocity=" + std::to_string(velocity_mps_) + " m/s, "
                "temperature=" + std::to_string(temperature_c_) + " C, "
                "voltage=" + std::to_string(voltage_v_) + " V"
            );


            while (current_time_ms_ < config_.end_time_ms) {
                auto frame = generate_frame();
                if (current_time_ms_ % 1000 == 0) {
                    logger_.info(
                        "t=" + std::to_string(current_time_ms_) +
                        "ms | pos=(" + std::to_string(position_x_) + ", "
                                    + std::to_string(position_y_) + ")"
                        + " | vel=" + std::to_string(velocity_mps_)
                        + " | temp=" + std::to_string(temperature_c_)
                        + " | volt=" + std::to_string(voltage_v_)
                    );
                }

                if (voltage_v_ < 11.5f) {
                    logger_.warn(
                        "Low voltage threshold breached: "
                        + std::to_string(voltage_v_) + " V"
                    );
                }

                if (std::abs(velocity_mps_) > 5.0f) {
                    logger_.warn(
                        "Velocity exceeds expected range: "
                        + std::to_string(velocity_mps_) + " m/s"
                    );
                }

                if (save_frames_) {
                    frames_.push_back(frame);
                }

                PacketWriter pw;
                frame.serialize(pw);
                recorder_.write_packet(pw.bytes());
                logger_.info(
                    "Packet written | size=" +
                    std::to_string(pw.bytes().size()) +
                    " bytes | timestamp=" +
                    std::to_string(frame.timestamp_ms)
                );


                advance_state();
            }

            recorder_.flush();
            logger_.info(
                "Simulation finished at t=" +
                std::to_string(current_time_ms_) +
                "ms | total_frames=" +
                std::to_string(frames_.size())
            );

        }


}