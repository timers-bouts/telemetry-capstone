#include "telemetry/TelemetryReader.h"
#include "telemetry/TelemetryFormat.h"
#include "telemetry/CRC.h"

#include <array>
#include <vector>
#include <span>
#include <string>
#include <cstring>   // std::memcmp
#include <utility>   // std::move

namespace telemetry {

    TelemetryReader::TelemetryReader(std::ifstream file,
                                    std::uint16_t version,
                                    std::uint16_t flags,
                                    Options opt,
                                    telemetry::Logger& logger)
        : file_(std::move(file)),
        version_(version),
        flags_(flags),
        opt_(std::move(opt)),
        logger_(logger)
        {}

        TelemetryReader TelemetryReader::open(const std::filesystem::path& path,
                                                telemetry::Logger& logger) {
            return TelemetryReader::open(path, Options{}, logger);
        }

    TelemetryReader TelemetryReader::open(const std::filesystem::path& path,
                                        Options opt,
                                        telemetry::Logger& logger) {

        // 1) Open std::ifstream in binary mode.
        //    If opening fails -> throw ParseError("...").

        std::ifstream in(path, std::ios::binary);

        if (!in) {
            logger.error("TelemetryReader failed to open file: " + path.string());
            throw ParseError("failed to open telemetry file");
        }

        logger.info("TelemetryReader opened file: " + path.string());

        // 2) Read 4-byte magic.
        //    If we can't read 4 bytes -> throw ParseError("truncated header (magic)").
        //    If magic != "TLRY" -> throw ParseError("invalid magic").
        std::array<std::uint8_t, format::kMagicSize> magic{};
        in.read(reinterpret_cast<char*>(magic.data()), format::kMagicSize);

        if (in.gcount() != static_cast<std::streamsize>(format::kMagicSize)) {
            logger.error("TelemetryReader truncated header (magic)");
            throw ParseError("truncated header (magic)");
        }

        if (magic != format::kMagic) {
            logger.error("TelemetryReader invalid magic");
            throw ParseError("invalid magic (expected 'TLRY')");
        }

        logger.info("TelemetryReader validated magic header");
        
        // 3) Read version (u16) and flags (u16).
        //    If either short-read -> throw ParseError("truncated header (version/flags)").
        
        // Read Version and Flags as little endian
        std::uint16_t version = read_u16_le(in);
        std::uint16_t flags = read_u16_le(in);

        // 4) Validate version supported (for now only 1).
        //    If unsupported -> throw ParseError("unsupported version").
        if (version != format::kCurrentVersion) {
            logger.error("TelemetryReader unsupported version: " + std::to_string(version));
            throw ParseError("unsupported version");
        }

        logger.info("TelemetryReader header OK | version=" +
            std::to_string(version) +
            " | flags=" +
            std::to_string(flags));

        // 5) Construct and return TelemetryReader by moving the stream + storing version/flags/options.
        //    return TelemetryReader(std::move(in), version, flags, opt);
        return TelemetryReader(std::move(in), version, flags, opt, logger);
    }

    bool TelemetryReader::read_next(std::vector<std::uint8_t>& out_packet) {
        // Try to read the size of the next payload. Return false if EOF
        std::array<std::uint8_t, format::kRecordSizeFieldBytes> packet_size{};
        if (!read_exact(file_, std::span<std::uint8_t>(packet_size))) {
            logger_.info("TelemetryReader reached clean EOF");
            return false;
        }
        // Able to read packet size (read_exact returns false only for clean EOF; otherwise it either fills the buffer or throws.)
        // Validate packet size (using little endian)
        uint32_t size = (static_cast<std::uint32_t>(packet_size[3])) << 24 | 
                        (static_cast<std::uint32_t>(packet_size[2])) << 16 |
                        (static_cast<std::uint32_t>(packet_size[1])) << 8 |
                        (static_cast<std::uint32_t>(packet_size[0]));
        
        if (size == static_cast<std::uint32_t>(0)) {
            logger_.error("TelemetryReader invalid packet size 0");
            throw ParseError("invalid packet size 0");
        } else if (size > opt_.max_packet_size) {
            logger_.error("TelemetryReader packet size exceeds limit: " +
                            std::to_string(size));
            throw ParseError("packet size " + std::to_string(size) + " exceeds max_packet_size");
        }

        //Prepare out_packet for reading payload (minus CRC bits)
        if (size < format::CRC32_SIZE) {
            logger_.error("TelemetryReader packet size too small for CRC");
            throw ParseError("packet size too small for CRC");
        }
        out_packet.resize(size - format::CRC32_SIZE);


        // Read payload into out_packet (read_exact will throw error if less than 'size' bytes are read)
        if (!read_exact(file_, std::span<std::uint8_t>(out_packet))) {
            logger_.error("TelemetryReader truncated packet payload");
            out_packet.clear(); // Clear out_packet on failure to read payload
            throw ParseError("truncated packet payload");
        }

        // Read the CRC bits and check against the actual CRC
        std::uint32_t crc_computed = crc32(std::span<std::uint8_t>(out_packet)); // CRC Computed from packet payload

        uint32_t crc_stored = read_u32_le(file_); // CRC stored at the end of the payload
        
        
        if (crc_computed != crc_stored) {
            logger_.error("TelemetryReader CRC mismatch");
            throw ParseError("incorrect CRC");
        }

        logger_.info("TelemetryReader read packet | payload=" +
                    std::to_string(out_packet.size()) +
                    " bytes");
        
        return true;

    }

    bool TelemetryReader::read_exact(std::istream& in, std::span<std::uint8_t> buf) {
        std::streamsize total = 0;
        std::streamsize target = static_cast<std::streamsize>(buf.size());
        while (total < target) {
            in.read(reinterpret_cast<char*>(buf.data() + total), static_cast<std::streamsize>(target - total));
            std::streamsize got = in.gcount();
            if (got == 0) {
                if (total == 0 && in.eof()) {
                    return false;
                } else {
                    throw ParseError("truncated input (unexpected EOF)");
                }
            }
            total += got;
        }
        return true;
    }

    std::uint16_t TelemetryReader::read_u16_le(std::istream& in) {
        std::array<std::uint8_t, 2> u16Buffer{};
        if (!read_exact(in, std::span<std::uint8_t>(u16Buffer))) {
            throw ParseError("unexpected EOF while reading u16");
        }

        return (static_cast<std::uint16_t>(u16Buffer[1]) << 8) | static_cast<std::uint16_t>(u16Buffer[0]);
    }

    std::uint32_t TelemetryReader::read_u32_le(std::istream& in) {
        std::array<std::uint8_t, 4> u32Buffer{};
        if (!read_exact(in, std::span<std::uint8_t>(u32Buffer))) {
            throw ParseError("unexpected EOF while reading u32");
        }

        return (static_cast<std::uint32_t>(u32Buffer[3])) << 24 | 
                (static_cast<std::uint32_t>(u32Buffer[2])) << 16 |
                (static_cast<std::uint32_t>(u32Buffer[1])) << 8 |
                (static_cast<std::uint32_t>(u32Buffer[0]));
    }

}