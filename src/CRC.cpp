#include "telemetry/CRC.h"

namespace telemetry {

    std::uint32_t crc32(const std::span<const std::uint8_t>& data) {
        std::uint32_t polynomial = 0x04C11DB7;
        std::uint32_t crc = 0xFFFFFFFF;

        for (std::uint8_t byte : data) {
            byte ^= static_cast<std::uint8_t>((crc >> 24) & 0xFF);
            crc = (crc & 0x00FFFFFF) | (static_cast<std::uint32_t>(byte) << 24);

            for (int i = 0; i < 8; i++) {
                if (crc & 0x80000000) {
                    crc = (crc << 1) ^ polynomial;
                } else {
                    crc <<= 1;
                }
            }
        }

        crc ^= 0xFFFFFFFF;
        return crc;
    }

}
