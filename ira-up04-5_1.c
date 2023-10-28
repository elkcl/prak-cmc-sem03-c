#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

void
my_write(int fd, void *x)
{
    int count = sizeof(int);
    char *ptr = x;
    errno = 0;
    int k = 1;
    while (k > 0 && count > 0) {
        k = write(fd, ptr, count);
        if (k == -1) {
            fprintf(stderr, "cannot write: %s", strerror(errno));
            exit(1);
        }
        count -= k;
        ptr += k;
    }
}

int
main(int argc, char **argv)
{
    int fd1 = open(argv[1], O_RDONLY);
    errno = 0;
    if (fd1 < 0) {
        fprintf(stderr, "%s cannot open %s: %s", argv[0], argv[1], strerror(errno));
        exit(1);
    }
    int fd2 = open(argv[2], O_WRONLY | O_CREAT, 0600);
    errno = 0;
    if (fd2 < 0) {
        fprintf(stderr, "%s cannot open %s: %s", argv[0], argv[2], strerror(errno));
        exit(1);
    }
    char *eptr = NULL;
    errno = 0;
    long long mod = strtol(argv[3], &eptr, 10);
    if (errno || *eptr || eptr == argv[3] || (long long) mod != mod) {
        if (eptr == argv[3]) {
            fprintf(stderr, "Empty string\n");
        } else if (errno) {
            fprintf(stderr, "Overflow\n");
        } else if (*eptr) {
            fprintf(stderr, "Trash safter string\n");
        } else {
            fprintf(stderr, "Not integer\n");
        }
        exit(1);
    }
    long long sum_squares = 0;
    off_t size = lseek(fd1, 0, SEEK_END);
    if (size < 0) {
        fprintf(stderr, "%s lseek error: %s", argv[0], strerror(errno));
        exit(1);
    }
    if (lseek(fd1, 0, SEEK_SET) == -1) {
        fprintf(stderr, "%s lseek error: %s", argv[0], strerror(errno));
        exit(1);
    }
    char a;
    int file_size = size / sizeof(a);
    for (int i = 0; i < file_size; i++) {
        int k = read(fd1, &a, sizeof(a));
        if (k < 0) {
            fprintf(stderr, "%s cannot read: %s\n", argv[0], strerror(errno));
            exit(1);
        }
        int count = 0;
        while (count != CHAR_BIT) {
            char x = a & 1;
            long long curr = (CHAR_BIT * i + count + 1) % mod;
            sum_squares = (sum_squares + (curr * curr) % mod) % mod;
            if (x) {
                my_write(fd2, &sum_squares);
            }
            a = a >> 1;
            count++;
        }
    }
    return 0;
}


