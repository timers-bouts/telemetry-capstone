#include <bit>
#include <format>
#include <cstring>

#include "telemetry/PacketReader.h"

namespace telemetry {

    PacketReader::PacketReader(std::span<const std::uint8_t> buffer,
                              Endianness endianness) noexcept 
                : buffer_(buffer),
                  endianness_(endianness)
                {}

    std::uint8_t PacketReader::read_u8() {
        if (empty()) {
            throw std::out_of_range("no remaining bytes to read");
        }
        std::uint8_t byte = buffer_[0];
        buffer_ = buffer_.subspan(1);
        return byte;
    }

    void PacketReader::read_bytes(std::span<std::uint8_t> dst) {
        if (dst.size() > remaining()) {
            throw std::out_of_range(
                std::format("trying to read {} bytes, but only {} bytes available",
                            dst.size(), remaining())
            );
        }
        
        if (dst.empty()) {
            return;
        }

        std::memcpy(dst.data(), buffer_.data(), dst.size());
        buffer_ = buffer_.subspan(dst.size());
    }

    std::uint16_t PacketReader::read_u16() {
        std::uint8_t b0 = read_u8();
        std::uint8_t b1 = read_u8();

        if (endianness_ == Endianness::Big) {
            return (static_cast<std::uint16_t>(b0) << 8) |
                    static_cast<std::uint16_t>(b1);
        }

        // Little endian
        return static_cast<std::uint16_t>(b0) |
            (static_cast<std::uint16_t>(b1) << 8);
    }

    std::uint32_t PacketReader::read_u32() {
        std::uint16_t b01 = read_u16();
        std::uint16_t b23 = read_u16();

        if (endianness_ == PacketReader::Endianness::Big) {
            return (static_cast<std::uint32_t>(b01) << 16) | static_cast<std::uint32_t>(b23);
        } else {
            // Little Endian
            return static_cast<std::uint32_t>(b01) | (static_cast<std::uint32_t>(b23) << 16);
        }
    }

    std::uint64_t PacketReader::read_u64() {
        std::uint32_t b0123 = read_u32();
        std::uint32_t b4567 = read_u32();

        if (endianness_ == PacketReader::Endianness::Big) {
            return (static_cast<std::uint64_t>(b0123) << 32) | static_cast<std::uint64_t>(b4567);
        } else {
            // Little Endian
            return static_cast<std::uint64_t>(b0123) | (static_cast<std::uint64_t>(b4567) << 32);
        }
    }

    float PacketReader::read_float() {
        // A float is stored as 4 bytes (1 sign bit, 23 for mantissa, 8 for exponent)
        return std::bit_cast<float>(read_u32());
    }


    [[nodiscard]] bool PacketReader::empty() const noexcept {
        return remaining() == 0;
    }

    [[nodiscard]] std::size_t PacketReader::remaining() const noexcept {
        return buffer_.size();
    }

    void PacketReader::skip(std::size_t count) {
        if (count > remaining()) {
            throw std::out_of_range(
                std::format("trying to skip {} bytes, but only {} bytes available",
                            count, remaining())
            );
        }

        buffer_ = buffer_.subspan(count);
    }


    void PacketReader::clear() noexcept {
        buffer_ = std::span<const uint8_t> {};
    }
    
} // namespace telemetry 