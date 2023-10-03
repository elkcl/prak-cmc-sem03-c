#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

enum CONSTANTS
{
    BYTE3 = 0xFF,
    SHIFT3 = 0,
    BYTE2 = 0x0F00,
    SHIFT2 = 8,
    BYTE1 = 0xFF000,
    SHIFT1 = 12,
    BYTE0 = 0x0F00000,
    SHIFT0 = 20,
    ARGC = 2,
    BUF_SIZE = 4,
    VALID_MASK = 0xF000000
};

int
main(int argc, char **argv)
{
    if (argc != ARGC) {
        fprintf(stderr, "Error: wrong number of arguments\n");
        exit(1);
    }
    int out = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (out == -1) {
        fprintf(stderr, "Error: couldn't open file\n");
        exit(1);
    }
    unsigned int curr = 0;
    unsigned char buf[BUF_SIZE];
    while (scanf("%u", &curr) > 0) {
        if (curr & VALID_MASK) {
            fprintf(stderr, "Error: incorrect integer format\n");
            exit(1);
        }
        buf[0] = (curr & BYTE0) >> SHIFT0;
        buf[1] = (curr & BYTE1) >> SHIFT1;
        buf[2] = (curr & BYTE2) >> SHIFT2;
        buf[3] = (curr & BYTE3) >> SHIFT3;
        if (write(out, buf, sizeof(buf)) == -1) {
            fprintf(stderr, "Error: couldn't write to file\n");
            exit(1);
        }
    }
    if (close(out) == -1) {
        fprintf(stderr, "Error: couldn't close file\n");
        exit(1);
    }
}
