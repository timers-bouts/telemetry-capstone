#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <vector>

#include "telemetry/Logger.h"

namespace telemetry {

    // Thrown on format/parse errors (bad magic, truncation, bad CRC, etc.)
    class ParseError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    class TelemetryReader {
        public:
            struct Options {
                std::size_t max_packet_size = 64 * 1024; // defensive cap (64 KiB)
            };

            // Open file + validate header. Throws ParseError on invalid format.
            static TelemetryReader open(const std::filesystem::path& path,
                                        telemetry::Logger& logger);

            static TelemetryReader open(const std::filesystem::path& path,
                                        Options opt,
                                        telemetry::Logger& logger);

            TelemetryReader(const TelemetryReader&) = delete;
            TelemetryReader& operator=(const TelemetryReader&) = delete;

            TelemetryReader(TelemetryReader&&) noexcept = default;
            TelemetryReader& operator=(TelemetryReader&&) noexcept = delete;

            ~TelemetryReader() = default;

            // Header info (after open()).
            std::uint16_t version() const noexcept { return version_; }
            std::uint16_t flags() const noexcept { return flags_; }

            // Read next framed packet:
            //   [u32 packet_size][packet_bytes...][u32 crc]
            //
            // Returns:
            //   true  -> packet read into out_packet
            //   false -> clean EOF (no more records)
            //
            // Throws ParseError on corruption/truncation/oversized packet.
            bool read_next(std::vector<std::uint8_t>& out_packet);

        private:
            TelemetryReader(std::ifstream file,
                            std::uint16_t version,
                            std::uint16_t flags,
                            Options opt,
                            telemetry::Logger& logger);

            // Reads exactly buf.size() bytes.
            // Returns false only if EOF occurs before reading any bytes.
            // Throws ParseError if a short read occurs after partial bytes.
            static bool read_exact(std::istream& in, std::span<std::uint8_t> buf);

            static std::uint16_t read_u16_le(std::istream& in);
            static std::uint32_t read_u32_le(std::istream& in);

            std::ifstream file_;
            std::uint16_t version_{0};
            std::uint16_t flags_{0};
            Options opt_;
            telemetry::Logger& logger_;
    };

} // namespace telemetry
