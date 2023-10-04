#include <asm-generic/errno-base.h> // EINTR
#include <asm-generic/errno.h>      // EWOULDBLOCK, EAGAIN
#include <stdio.h>                  // fprintf
#include <string.h>                 // strerror
#include <sys/types.h>              // size_t
#include <fcntl.h>                  // open()
#include <unistd.h>                 // close()
#include <stdlib.h>                 // exit()
#include <errno.h>                  // errno

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
        goto CLOSE_AND_EXIT;
    }
    for (long long i = 1; i < n; ++i) {
        double curr = 0;
        cnt_read = read(file, &curr, sizeof(curr));
        if (cnt_read && cnt_read != sizeof(curr)) {
            panic("reading from file failed or file format incorrect");
        }
        if (cnt_read == 0) {
            goto CLOSE_AND_EXIT;
        }
        if (lseek(file, i * sizeof(double), SEEK_SET) == -1) {
            panic("seeking failed");
        }
        curr -= prev;
        write_all(file, &curr, sizeof(double));
        prev = curr;
    }

CLOSE_AND_EXIT:;
    if (close(file) == -1) {
        panic("closing file failed");
    }
    return 0;
}
