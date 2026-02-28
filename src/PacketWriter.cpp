#include <bit>

#include "telemetry/PacketWriter.h"

namespace telemetry {

    PacketWriter::PacketWriter(PacketWriter::Endianness endianness)
        :endianness_(endianness){}

    PacketWriter::~PacketWriter() = default;

    PacketWriter& PacketWriter::add_u8(std::uint8_t byte) {
        buffer_.push_back(byte);
        return *this;
    }

    PacketWriter& PacketWriter::add_u16(std::uint16_t value) {
        if (endianness_ == PacketWriter::Endianness::Big) {
            add_u8(static_cast<uint8_t>((value >> 8) & 0xFF));
            add_u8(static_cast<uint8_t>(value & 0xFF));
        } else {
            add_u8(static_cast<uint8_t>(value & 0xFF));
            add_u8(static_cast<uint8_t>((value >> 8) & 0xFF));
        }
        return *this;
    }

    PacketWriter& PacketWriter::add_u32(std::uint32_t value) {
        if (endianness_ == PacketWriter::Endianness::Big) {
            add_u16(static_cast<uint16_t>((value >> 16) & 0xFFFF));
            add_u16(static_cast<uint16_t>(value & 0xFFFF));
        } else {
            add_u16(static_cast<uint16_t>(value & 0xFFFF));
            add_u16(static_cast<uint16_t>((value >> 16) & 0xFFFF));

        }
        return *this;
    }

    PacketWriter& PacketWriter::add_u64(std::uint64_t value) {
        if (endianness_ == PacketWriter::Endianness::Big) {
            add_u32(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
            add_u32(static_cast<uint32_t>(value & 0xFFFFFFFF));
        } else {
            add_u32(static_cast<uint32_t>(value & 0xFFFFFFFF));
            add_u32(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
        }

        return *this;
}

    PacketWriter& PacketWriter::add_bytes(std::span<const uint8_t> bytes)  {
        buffer_.reserve(buffer_.size() + bytes.size());
        buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());     
        return *this;
    }

    PacketWriter& PacketWriter::add_string(std::string_view s) {
        if (s.size() > 65535) {
            throw std::length_error("string too long for u16 length prefix");
        }

        add_u16(static_cast<std::uint16_t>(s.size()));

        auto ptr = reinterpret_cast<const std::uint8_t*>(s.data());
        add_bytes(std::span<const std::uint8_t>(ptr, s.size()));

        return *this;
    }

    PacketWriter& PacketWriter::add_float(float ft) {
        uint32_t bits = std::bit_cast<uint32_t>(ft);
        add_u32(bits);
        return *this;
    }

    void PacketWriter::clear() {
        buffer_.clear();
    }

    std::size_t PacketWriter::size() const {
        return buffer_.size();
    }

    std::span<const std::uint8_t> PacketWriter::bytes() const {
        return std::span<const std::uint8_t>(buffer_.data(), buffer_.size());
    }

    void PacketWriter::reserve(std::size_t n) {
        buffer_.reserve(n);
    }
}