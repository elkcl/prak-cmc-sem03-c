#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

enum
{
    INT_BYTES = 4,
    INT_BITS = 32
};

struct Node
{
    int32_t key;
    int32_t left_idx;
    int32_t right_idx;
};

ssize_t
my_read(int fd, void *key)
{
    size_t size = sizeof(int32_t);
    size_t k = 1;
    unsigned char bytes[INT_BYTES];
    unsigned char *ptr = bytes;
    int32_t *res = key;
    errno = 0;
    while (k > 0 && size > 0) {
        k = read(fd, ptr, size);
        if (k == -1) {
            fprintf(stderr, "cannot read: %s\n", strerror(errno));
            exit(1);
        }
        ptr += k;
        size -= k;
    }
    int32_t val = (uint32_t) bytes[3] | (uint32_t) bytes[2] << 8 | (uint32_t) bytes[1] << 16 | (uint32_t) bytes[0] << 24;
    *res = val;
    //printf("%zu %d\n", size, val);
    return INT_BYTES - size;
}

void
find_node(int fd)
{
    int32_t key, left, right;
    errno = 0;
    if (my_read(fd, &key) != sizeof(key)) {
        fprintf(stderr, "cannot read root: %s\n", strerror(errno));
        exit(1);
    }
    //printf("key: %d\n", key);
    errno = 0;
    if (my_read(fd, &left) != sizeof(left)) {
        fprintf(stderr, "cannot read left: %s\n", strerror(errno));
        exit(1);
    }
    errno = 0;
    if (my_read(fd, &right) != sizeof(right)) {
        fprintf(stderr, "cannot read right: %s\n", strerror(errno));
        exit(1);
    }
    errno = 0;
    if (right != 0) {
        //printf("read right\n");
        if (lseek(fd, (off_t) right * sizeof(struct Node), SEEK_SET) == (off_t) -1) {
            fprintf(stderr, "wrong lseek: %s\n", strerror(errno));
            exit(1);
        }
        find_node(fd);
    }
    printf("%d\n", key);
    errno = 0;
    if (left != 0) {
        // printf("read left");
        if (lseek(fd, (off_t) left * sizeof(struct Node), SEEK_SET) == (off_t) -1) {
            fprintf(stderr, "wrong lseek: %s\n", strerror(errno));
            exit(1);
        }
        find_node(fd);
    }
}

int
main(int argc, char **argv)
{
    errno = 0;
    int fd = open(argv[1], O_RDONLY, 0666);
    if (fd < 0) {
        fprintf(stderr, "%s: cannot open file '%s': %s\n", argv[0], argv[1], strerror(errno));
        exit(1);
    }
    find_node(fd);
    close(fd);
    return 0;
}

