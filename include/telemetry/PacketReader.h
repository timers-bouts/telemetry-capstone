#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace telemetry {

    /// @brief Utility class to read primitive types from a serialized packet buffer.
    ///
    /// Reads data in the same order as PacketWriter writes them. Supports
    /// little- and big-endian formats.
    class PacketReader {
    public:
        enum class Endianness {
            Little,
            Big
        };

        /// @brief Construct a PacketReader over a contiguous buffer.
        /// @param buffer Serialized packet data.
        /// @param endianness Endianness of the serialized data (default Big to match PacketWriter).
        explicit PacketReader(std::span<const std::uint8_t> buffer,
                              Endianness endianness = Endianness::Big) noexcept;

        ~PacketReader() = default;

        /// @brief Returns the number of bytes remaining to read.
        [[nodiscard]] std::size_t remaining() const noexcept;

        /// @brief Returns true if all bytes have been read.
        [[nodiscard]] bool empty() const noexcept;

        /// @brief Reads an unsigned 8-bit value from the buffer.
        std::uint8_t read_u8();

        /// @brief Reads an unsigned 16-bit value from the buffer.
        std::uint16_t read_u16();

        /// @brief Reads an unsigned 32-bit value from the buffer.
        std::uint32_t read_u32();

        /// @brief Reads an unsigned 64-bit value from the buffer.
        std::uint64_t read_u64();

        /// @brief Reads a single-precision float from the buffer.
        float read_float();

        /// @brief Reads raw bytes into a destination buffer.
        /// @param dst Span to fill.
        void read_bytes(std::span<std::uint8_t> dst);

        /// @brief Skips a number of bytes in the buffer.
        /// @param count Number of bytes to skip.
        void skip(std::size_t count);

        /// @brief Clears the buffer and resets reading position.
        void clear() noexcept;

    private:
        std::span<const std::uint8_t> buffer_;
        Endianness endianness_;
    };

} // namespace telemetry
