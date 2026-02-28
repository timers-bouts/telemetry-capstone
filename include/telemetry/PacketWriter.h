#pragma once

#include <vector>
#include <cstdint>
#include <span>
#include <cstddef>
#include <string_view>

namespace telemetry {

    class PacketWriter {
        public:
            enum class Endianness {
                Little,
                Big
            };

            explicit PacketWriter(Endianness endianness = Endianness::Big);
            ~PacketWriter();

            PacketWriter& add_u8(std::uint8_t);
            PacketWriter& add_u16(std::uint16_t);
            PacketWriter& add_u32(std::uint32_t);
            PacketWriter& add_u64(std::uint64_t);
            PacketWriter& add_bytes(std::span<const uint8_t>);
            PacketWriter& add_string(std::string_view);
            PacketWriter& add_float(float);

            std::span<const std::uint8_t> bytes() const;
            void clear();
            std::size_t size() const;
            void reserve(std::size_t);
        private:
            std::vector<std::uint8_t> buffer_;
            Endianness endianness_;
    };

} // namespace telemetry
