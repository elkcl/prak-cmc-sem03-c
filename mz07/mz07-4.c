#include <limits.h>
#include <stdio.h>    // fprintf()
#include <string.h>   // strerror()
#include <errno.h>    // errno
#include <stdlib.h>   // exit()
#include <sys/stat.h> // stat()
#include <dirent.h>   // dirent
#include <unistd.h>   // access()
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>

void
panic(const char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    exit(1);
}

enum
{
    ARGC = 2,
    FILE_ARG = 1,
};

int
main(int argc, char **argv)
{
    if (argc != ARGC || argv == NULL || argv[FILE_ARG] == NULL) {
        panic("incorrect argument format");
    }
    int fd = open(argv[FILE_ARG], O_RDWR);
    if (fd == -1) {
        panic("couldn't open file");
    }
    const off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        close(fd);
        panic("couldn't get file size");
    }
    int32_t num;
    while (scanf("%" PRId32, &num) == 1) {
        if (num > 0 && num - 1 < size * CHAR_BIT) {
            const uint32_t byte = (num - 1) / CHAR_BIT;
            const uint32_t bit = (num - 1) % CHAR_BIT;
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                panic("couldn't seek");
            }
            unsigned char curr;
            if (read(fd, &curr, sizeof(curr)) != sizeof(curr)) {
                close(fd);
                panic("couldn't read");
            }
            curr |= 1u << bit;
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                panic("couldn't seek");
            }
            if (write(fd, &curr, sizeof(curr)) != sizeof(curr)) {
                close(fd);
                panic("couldn't write");
            }
        } else if (num < 0 && -(num + 1) < size * CHAR_BIT) {
            const uint32_t byte = -(num + 1) / CHAR_BIT;
            const uint32_t bit = -(num + 1) % CHAR_BIT;
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                panic("couldn't seek");
            }
            unsigned char curr;
            if (read(fd, &curr, sizeof(curr)) != sizeof(curr)) {
                close(fd);
                panic("couldn't read");
            }
            curr &= ~(1u << bit);
            if (lseek(fd, byte, SEEK_SET) == -1) {
                close(fd);
                panic("couldn't seek");
            }
            if (write(fd, &curr, sizeof(curr)) != sizeof(curr)) {
                close(fd);
                panic("couldn't write");
            }
        }
    }
    if (close(fd) == -1) {
        panic("couldn't close file");
    }
}
