#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

pid_t
ping_pong_player(int id, int lim, int fd_out[static 2], int fd_in[static 2])
{
    pid_t pid = fork();
    if (pid < 0) {
        panic("fork failed");
    } else if (pid == 0) {
        if (close(fd_out[0]) == -1) {
            panic("close failed");
        }
        if (close(fd_in[1]) == -1) {
            panic("close failed");
        }
        FILE *in = fdopen(fd_in[0], "r");
        if (in == NULL) {
            panic("fdopen failed");
        }
        setbuf(in, NULL);
        FILE *out = fdopen(fd_out[1], "w");
        if (out == NULL) {
            panic("fdopen failed");
        }
        setbuf(out, NULL);
        int x;
        while (fscanf(in, "%d", &x) == 1) {
            if (x >= lim) {
                break;
            }
            printf("%d %d\n", id, x);
            ++x;
            if (x >= lim) {
                break;
            }
            fprintf(out, "%d\n", x);
        }
        if (fclose(in) != 0) {
            panic("close failed");
        }
        if (fclose(out) != 0) {
            panic("close failed");
        }
        _exit(0);
    }
    return pid;
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
    ping_pong_player(1, lim, fd1, fd2);
    ping_pong_player(2, lim, fd2, fd1);
    dprintf(fd2[1], "%d\n", 1);
    if (close(fd1[0]) == -1) {
        panic("close failed");
    }
    if (close(fd1[1]) == -1) {
        panic("close failed");
    }
    if (close(fd2[0]) == -1) {
        panic("close failed");
    }
    if (close(fd2[1]) == -1) {
        panic("close failed");
    }
    while (wait(NULL) > 0) {
    }
    puts("Done");
}
