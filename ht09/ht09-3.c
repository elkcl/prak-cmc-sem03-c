#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
    CRC8_BITS = 8,
    CRC8_TABLE_SIZE = 1u << CRC8_BITS,
    CRC8_MSB_MASK = 0x80,
    CRC8_GENERATOR = 0x1D
};

uint8_t crc8_table[CRC8_TABLE_SIZE];
bool crc8_table_precomputed = false;

void
crc8_table_precompute()
{
    for (int d = 0; d < CRC8_TABLE_SIZE; ++d) {
        uint8_t curr = d;
        for (uint8_t bit = 0; bit < CRC8_BITS; ++bit) {
            if (curr & CRC8_MSB_MASK) {
                curr <<= 1;
                curr ^= CRC8_GENERATOR;
            } else {
                curr <<= 1;
            }
        }
        crc8_table[d] = curr;
    }
    crc8_table_precomputed = true;
}

uint8_t
compute_crc8(uint8_t *data, size_t size)
{
    if (!crc8_table_precomputed) {
        crc8_table_precompute();
    }
    uint8_t crc = 0;
    for (size_t i = 0; i < size; ++i) {
        uint8_t curr = data[i] ^ crc;
        crc = crc8_table[curr];
    }
    return crc;
}
