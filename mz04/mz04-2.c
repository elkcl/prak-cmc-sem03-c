#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

enum
{
    ARGC = 3,
    BASE = 10,
    FILE_ARG = 1,
    N_ARG = 2,
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

int
main(int argc, char **argv)
{
    if (argc != ARGC) {
        panic("incorrect number of arguments");
    }
    errno = 0;
    char *eptr = NULL;
    long long n = strtoll(argv[N_ARG], &eptr, BASE);
    if (errno || *eptr || eptr == argv[N_ARG]) {
        panic("integer conversion failed");
    }
    if (n <= 1) {
        return 0;
    }
    int file = open(argv[FILE_ARG], O_RDWR);
    if (file == -1) {
        panic("opening file failed");
    }
    double prev = 0;
    ssize_t cnt_read = read(file, &prev, sizeof(prev));
    if (cnt_read && cnt_read != sizeof(prev)) {
        panic("reading from file failed or file format incorrect");
    }
    if (cnt_read == 0) {
        return 0;
    }
    for (long long i = 1; i < n; ++i) {
        double curr = 0;
        cnt_read = read(file, &curr, sizeof(curr));
        if (cnt_read && cnt_read != sizeof(curr)) {
            panic("reading from file failed or file format incorrect");
        }
        if (lseek(file, i * sizeof(double), SEEK_SET) == -1) {
            panic("seeking failed");
        }
        curr -= prev;
        ssize_t cnt_written = write(file, &curr, sizeof(double));
        if (cnt_read == 0) {
            break;
        }
    }

    if (close(file) == -1) {
        panic("closing file failed");
    }
}
