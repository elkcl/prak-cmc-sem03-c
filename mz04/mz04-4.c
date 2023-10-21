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

typedef unsigned char num_t[sizeof(int32_t)];

typedef struct
{
    num_t key, left_idx, right_idx;
} Node;

enum
{
    ARGC = 2,
    FILE_ARG = 1
};

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
read_all(int fd, void *buf_v, size_t size)
{
    char *buf = buf_v;
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

int32_t
num_convert(num_t n)
{
    uint32_t byte0 = (uint32_t) n[3];
    uint32_t byte1 = (uint32_t) n[2] << CHAR_BIT;
    uint32_t byte2 = (uint32_t) n[1] << CHAR_BIT * 2;
    uint32_t byte3 = (uint32_t) n[0] << CHAR_BIT * 3;
    uint32_t ans = byte0 | byte1 | byte2 | byte3;
    return ans;
}

void
print_tree(int fd, int32_t idx)
{
    if (lseek(fd, idx * sizeof(Node), SEEK_SET) == -1) {
        panic("seeking failed");
    }
    Node v = {};
    ssize_t cnt_read = read_all(fd, &v, sizeof(v));
    if (idx == 0 && cnt_read == 0) {
        return;
    }
    if (cnt_read != sizeof(v)) {
        panic("file format is incorrect");
    }
    int32_t key = num_convert(v.key);
    int32_t left_idx = num_convert(v.left_idx);
    int32_t right_idx = num_convert(v.right_idx);
    if (right_idx) {
        print_tree(fd, right_idx);
    }
    printf("%d\n", key);
    if (left_idx) {
        print_tree(fd, left_idx);
    }
}

int
main(int argc, char **argv)
{
    if (argc != ARGC) {
        panic("incorrect number of arguments");
    }
    int fd = open(argv[FILE_ARG], O_RDONLY);
    if (fd == -1) {
        panic("opening file failed");
    }
    print_tree(fd, 0);
    if (close(fd) == -1) {
        panic("closing file failed");
    }
    return 0;
}
