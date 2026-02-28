#include <stdexcept>
#include <array>
#include <cstdint>
#include <limits>
#include <filesystem>
#include <sstream>
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetryFormat.h"
#include "telemetry/CRC.h"

// File format:
// [TLRY][u16 version][u16 flags]
// Repeated records: [u32 size][payload bytes...][u32 crc]

namespace telemetry {

    TelemetryRecorder::TelemetryRecorder(const std::filesystem::path& path,
                        telemetry::Logger& logger,
                        OpenMode mode = OpenMode::Truncate)
        : logger_(logger)
        {

        bool empty = !std::filesystem::exists(path) || std::filesystem::file_size(path) == 0;

        if (mode == TelemetryRecorder::OpenMode::Append) {
            out_.open(path, std::ios::binary | std::ios::out | std::ios::app);
        } else {
            out_.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
        }

        if (!out_.is_open()) {
            logger_.error("TelemetryRecorder failed to open file: " + path.string());
            throw std::runtime_error("Telemetry Recorder: Failed to open file: " + path.string());
        }

        logger_.info("TelemetryRecorder opened file: " + path.string());

        // Turn on fail and bad bits for error detection
        out_.exceptions(std::ios::failbit | std::ios::badbit);

        // Write the header //
        if (mode == TelemetryRecorder::OpenMode::Truncate || empty) {

            // 'Magic'
            out_.write(reinterpret_cast<const char*>(format::kMagic.data()), static_cast<std::streamsize>(format::kMagicSize));

            // Version
            std::uint16_t version = format::kCurrentVersion;
            out_.write(reinterpret_cast<const char*>(&version), sizeof(version));

            // Flags
            std::uint16_t flags = 0;
            out_.write(reinterpret_cast<const char*>(&flags), sizeof(flags));

            logger_.info("TelemetryRecorder wrote file header (magic/version/flags)");
        } else {
            logger_.info("TelemetryRecorder appending to existing file");
        }

    }

    TelemetryRecorder::~TelemetryRecorder() {
        if (out_.is_open()) {
            logger_.info("TelemetryRecorder closing file");
            out_.close();
        }
    }

    void TelemetryRecorder::write_u32_le(std::uint32_t value) {
        // Write using little endian
        std::array<uint8_t, 4> bytes = {static_cast<std::uint8_t>(value & 0xFF), static_cast<std::uint8_t>((value >> 8) & 0xFF),
                                        static_cast<std::uint8_t>((value >> 16) & 0xFF), static_cast<std::uint8_t>((value >> 24) & 0xFF)};
        out_.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(4));

    }

    void TelemetryRecorder::write_packet(std::span<const std::uint8_t> packet_bytes) {

        // Calculate packet size (inlcuding CRC) and the CRC value itself
        std::size_t size = packet_bytes.size() + format::CRC32_SIZE;
        std::uint32_t crc = crc32(packet_bytes);
        
        if (size > std::numeric_limits<std::uint32_t>::max()) {
            logger_.error("TelemetryRecorder packet size exceeds uint32_t limit");
            throw std::runtime_error("Packet size exceeds limits for uint32_t");
        }

        try {
            // Write the size of the payload
            write_u32_le(static_cast<std::uint32_t>(size));
            // Write the actual payload
            out_.write(reinterpret_cast<const char*>(packet_bytes.data()), static_cast<std::streamsize>(packet_bytes.size()));
            // Write the CRC at the end of the payload
            write_u32_le(crc);

            std::ostringstream oss;
            oss << std::hex << std::uppercase << crc;
            logger_.info(
                "TelemetryRecorder wrote packet | payload=" +
                std::to_string(packet_bytes.size()) +
                " bytes | crc=0x" + oss.str()
            );


        }
        catch (const std::ios_base::failure&) {
            logger_.error("TelemetryRecorder write failed (I/O exception)");
            throw std::runtime_error("TelemetryRecorder: write failed");
        } 
    }

    void TelemetryRecorder::flush() {
        out_.flush();
        logger_.info("TelemetryRecorder flushed output stream");
    }

}