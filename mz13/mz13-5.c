#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* ifndef _GNU_SOURCE */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum
{
    ARGC = 2,
    LIM_ARG = 1,
    BASE = 10,
};

void
panic(char *err)
{
    if (errno) {
        dprintf(STDERR_FILENO, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        dprintf(STDERR_FILENO, "Error: %s\n", err);
    }
    _exit(1);
}

void
ping_pong_player(int id, int lim, int fd_out[static 2], int fd_in[static 2])
{
    if (close(fd_out[0]) == -1) {
        panic("close failed");
    }
    if (close(fd_in[1]) == -1) {
        panic("close failed");
    }
    int x;
    while (dscanf(fd_in[0], "%d", &x) == 1) {
        printf("%d %d\n", id, x);
        ++x;
        if (x >= lim) {
            break;
        }
        dprintf(fd_out[1], "%d\n", x);
    }
    if (close(fd_out[1]) == -1) {
        panic("close failed");
    }
    if (close(fd_in[0]) == -1) {
        panic("close failed");
    }

}

int
main(int argc, char *argv[])
{
    if (argc != ARGC) {
        panic("incorrect input format");
    }
    char *eptr = NULL;
    errno = 0;
    long lim = strtol(argv[LIM_ARG], &eptr, BASE);
    if (errno || *eptr || eptr == argv[LIM_ARG]) {
        panic("incorrect input format");
    }
    int fd1[2], fd2[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        panic("pipe failed");
    }
    pid_t pid1 = fork();
    if (pid1 < 0) {
        panic("fork failed");
    } else if (pid1 == 0) {
    }
}

