#include <asm-generic/errno-base.h> // EINTR
#include <asm-generic/errno.h>      // EWOULDBLOCK, EAGAIN
#include <limits.h>                 // CHAR_BIT
#include <stdint.h>                 // int32_t
#include <stdio.h>                  // fprintf
#include <string.h>                 // strerror
#include <sys/types.h>              // size_t
#include <fcntl.h>                  // open()
#include <unistd.h>                 // close()
#include <stdlib.h>                 // exit()
#include <errno.h>                  // errno

enum
{
    ARGC = 4,
    FILE_IN_ARG = 1,
    FILE_OUT_ARG = 2,
    MOD_ARG = 3,
    FILE_MODE = 0644,
    BASE = 10,
    BUF_IN_SIZE = 1024,
    BUF_OUT_SIZE = BUF_IN_SIZE / sizeof(int32_t)
};

typedef unsigned long long num_t;

void
panic(char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    exit(1);
}

ssize_t
read_all(int fd, void *buf, size_t size)
{
    ssize_t remaining = size;
    while (remaining > 0) {
        ssize_t cnt_read = 0;
        do {
            errno = 0;
            cnt_read = read(fd, buf, remaining);
        } while (cnt_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
        if (cnt_read == -1) {
            panic("reading from file failed");
        }
        if (cnt_read == 0) {
            break;
        }
        remaining -= cnt_read;
        buf += cnt_read;
    }
    return size - remaining;
}

void
write_all(int fd, const void *buf, size_t size)
{
    while (size > 0) {
        ssize_t written = 0;
        do {
            errno = 0;
            written = write(fd, buf, size);
        } while (written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
        if (written == -1) {
            panic("writing to file failed");
        }
        size -= written;
        buf += written;
    }
}

num_t
sum(num_t x, num_t mod)
{
    // sum(x) = (x * (x + 1) * (2x + 1) / 6) % mod
    num_t k1 = x;
    num_t k2 = x + 1;
    num_t k3 = (x << 1) + 1;
    if (x & 1) {
        k2 >>= 1;
    } else {
        k1 >>= 1;
    }
    switch (x % 3) {
    case 0:
        k1 /= 3;
        break;
    case 1:
        k3 /= 3;
        break;
    case 2:
        k2 /= 3;
        break;
    }
    k1 %= mod;
    k2 %= mod;
    k3 %= mod;
    return ((k1 * k2) % mod * k3) % mod;
}

int
main(int argc, char **argv)
{
    if (argc != ARGC) {
        panic("incorrect number of arguments");
    }
    int file_in = open(argv[FILE_IN_ARG], O_RDONLY);
    if (file_in == -1) {
        panic("opening file failed");
    }
    int file_out = creat(argv[FILE_OUT_ARG], FILE_MODE);
    if (file_out == -1) {
        panic("closing file failed");
    }
    errno = 0;
    char *eptr = NULL;
    long str_mod = strtol(argv[MOD_ARG], &eptr, BASE);
    if (errno || *eptr || eptr == argv[MOD_ARG] || str_mod <= 0 || str_mod > INT32_MAX) {
        panic("incorrect integer format");
    }
    num_t mod = str_mod;
    unsigned char buf_in[BUF_IN_SIZE];
    int32_t buf_out[BUF_OUT_SIZE];
    size_t buf_out_idx = 0;
    num_t offset = 1;
    ssize_t cnt_read = 0;
    while ((cnt_read = read_all(file_in, buf_in, sizeof(buf_in)))) {
        for (ssize_t i = 0; i < cnt_read; ++i) {
            for (int j = 0; j < CHAR_BIT; ++j) {
                if (buf_in[i] & (1u << j)) {
                    buf_out[buf_out_idx] = sum(offset + j, mod);
                    ++buf_out_idx;
                    if (buf_out_idx == BUF_OUT_SIZE) {
                        write_all(file_out, buf_out, sizeof(buf_out));
                        buf_out_idx = 0;
                    }
                }
            }
            offset += CHAR_BIT;
        }
    }
    write_all(file_out, buf_out, buf_out_idx * sizeof(buf_out[0]));
    if (close(file_in) == -1) {
        panic("closing file failed");
    }
    if (close(file_out) == -1) {
        panic("closing file failed");
    }
}
