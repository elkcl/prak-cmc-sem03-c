#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum
{
    ULEB128_VAL_BITS = 64,
    ULEB128_CHUNK_BITS = 7,
    ULEB128_MSB = 0x80,
    ULEB128_CHUNK_MASK = 0x7F
};

struct ReadContext
{
    uint8_t *buf;
    size_t size;
    size_t pos;
    uint64_t value_u64;
};

size_t
uleb128_len(uint64_t value)
{
    if (value == 0) {
        return 1;
    }
    size_t bits = ULEB128_VAL_BITS - __builtin_clzll(value);
    size_t ans = bits / ULEB128_CHUNK_BITS;
    if (bits % ULEB128_CHUNK_BITS != 0) {
        ++ans;
    }
    return ans;
}

uint8_t *
uleb128_encode(uint8_t *ptr, uint64_t value)
{
    if (value == 0) {
        *ptr = 0;
        return ptr + 1;
    }
    while (value != 0) {
        ptr[0] = value & ULEB128_CHUNK_MASK;
        ptr[0] |= ULEB128_MSB;
        ++ptr;
        value >>= ULEB128_CHUNK_BITS;
    }
    ptr[-1] ^= ULEB128_MSB;
    return ptr;
}

bool
uleb128_decode(struct ReadContext *cntx)
{
    uint64_t ans = 0;
    bool last_byte = false;
    uint64_t shift = 0;
    size_t pos = cntx->pos;
    for (; !last_byte; ++pos, shift += ULEB128_CHUNK_BITS) {
        if (pos >= cntx->size) {
            return false;
        }
        uint8_t curr = cntx->buf[pos];
        if ((curr & ULEB128_MSB) == 0) {
            last_byte = true;
        }
        curr &= ULEB128_CHUNK_MASK;
        if (shift + ULEB128_CHUNK_BITS > ULEB128_VAL_BITS &&
            !(shift + 1 <= ULEB128_VAL_BITS && (curr == 0 || curr == 1))) {
            return false;
        }
        if (last_byte && shift != 0 && curr == 0) {
            return false;
        }
        ans |= (uint64_t) curr << shift;
    }
    cntx->pos = pos;
    cntx->value_u64 = ans;
    return true;
}
