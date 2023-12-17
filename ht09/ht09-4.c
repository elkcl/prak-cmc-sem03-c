#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const char B64U_ENCODE_LOOKUP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
const uint16_t B64U_DECODE_LOOKUP[] = {
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 62,    65535, 65535,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    65535, 65535, 65535, 65535, 65535, 65535,
    65535, 0,     1,     2,     3,     4,     5,     6,     7,     8,     9,     10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,    65535, 65535, 65535, 65535, 63,
    65535, 26,    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,    65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
    65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535};

enum
{
    B64U_MASK = 0x3F,
    B64U_UPPERCASE_OFFSET = 0,
    B64U_LOWERCASE_OFFSET = 26,
    B64U_DIGIT_OFFSET = 52,
    B64U_HYPHEN_OFFSET = 62,
    B64U_UNDERSCORE_OFFSET = 63,
    B64U_CLUSTER_BYTES = 3,
    B64U_CLUSTER_CHARS = 4
};

unsigned char
b64u_single_encode(uint8_t x)
{
    x &= B64U_MASK;
    return B64U_ENCODE_LOOKUP[x];
}

uint16_t
b64u_single_decode(unsigned char x)
{
    return B64U_DECODE_LOOKUP[x];
}

char *
b64u_encode(const uint8_t *data, size_t size)
{
    if (data == NULL) {
        return NULL;
    }
    size_t size_ans = (size / B64U_CLUSTER_BYTES) * B64U_CLUSTER_CHARS;
    if (size % B64U_CLUSTER_BYTES != 0) {
        size_ans += size % B64U_CLUSTER_BYTES + 1;
    }
    char *str = calloc(size_ans + 1, sizeof(char));
    if (str == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < size / B64U_CLUSTER_BYTES; ++i) {
        size_t di = i * B64U_CLUSTER_BYTES;
        size_t ci = i * B64U_CLUSTER_CHARS;
        str[ci] = b64u_single_encode(data[di] >> 2);
        str[ci + 1] = b64u_single_encode(data[di] << 4 | data[di + 1] >> 4);
        str[ci + 2] = b64u_single_encode(data[di + 1] << 2 | data[di + 2] >> 6);
        str[ci + 3] = b64u_single_encode(data[di + 2]);
    }
    if (size % B64U_CLUSTER_BYTES == 2) {
        size_t di = size - 2;
        size_t ci = size_ans - 3;
        str[ci] = b64u_single_encode(data[di] >> 2);
        str[ci + 1] = b64u_single_encode(data[di] << 4 | data[di + 1] >> 4);
        str[ci + 2] = b64u_single_encode(data[di + 1] << 2);
    } else if (size % B64U_CLUSTER_BYTES == 1) {
        size_t di = size - 1;
        size_t ci = size_ans - 2;
        str[ci] = b64u_single_encode(data[di] >> 2);
        str[ci + 1] = b64u_single_encode(data[di] << 4);
    }
    str[size_ans] = '\0';
    return str;
}

bool
b64u_decode(const char *str, uint8_t **p_data, size_t *p_size)
{
    if (str == NULL || p_data == NULL || p_size == NULL) {
        return false;
    }
    size_t char_cnt = 0;
    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (!isspace(str[i])) {
            if (b64u_single_decode(str[i]) == (uint16_t) -1) {
                return false;
            }
            ++char_cnt;
        }
    }
    size_t leftover = char_cnt % B64U_CLUSTER_CHARS;
    if (leftover == 1) {
        return false;
    }
    size_t size = (char_cnt / B64U_CLUSTER_CHARS) * B64U_CLUSTER_BYTES;
    if (leftover > 0) {
        size += leftover - 1;
    }
    uint8_t *data = calloc(size + 1, sizeof(*data));
    *p_data = data;
    *p_size = size;
    if (data == NULL) {
        return false;
    }
    data[size] = 0;
    size_t ci = 0;
    uint8_t cluster[B64U_CLUSTER_CHARS] = {};
    for (size_t i = 0; i < char_cnt / B64U_CLUSTER_CHARS; ++i) {
        size_t di = i * B64U_CLUSTER_BYTES;
        for (size_t j = 0; j < B64U_CLUSTER_CHARS; ++j) {
            while (isspace(str[ci])) {
                ++ci;
            }
            cluster[j] = b64u_single_decode(str[ci]);
            ++ci;
        }
        data[di] = cluster[0] << 2 | cluster[1] >> 4;
        data[di + 1] = cluster[1] << 4 | cluster[2] >> 2;
        data[di + 2] = cluster[2] << 6 | cluster[3];
    }
    for (size_t j = 0; j < leftover; ++j) {
        while (isspace(str[ci])) {
            ++ci;
        }
        cluster[j] = b64u_single_decode(str[ci]);
        ++ci;
    }
    if (leftover == 3) {
        data[size - 2] = cluster[0] << 2 | cluster[1] >> 4;
        data[size - 1] = cluster[1] << 4 | cluster[2] >> 2;
    } else if (leftover == 2) {
        data[size - 1] = cluster[0] << 2 | cluster[1] >> 4;
    }
    return true;
}
