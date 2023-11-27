#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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

int
main(int argc, char *argv[])
{
    int fd[2];
    if (pipe(fd) == -1) {
        panic("pipe failed");
    }
    pid_t pid_ch = fork();
    if (pid_ch < 0) {
        panic("fork failed");
    } else if (pid_ch == 0) {
        pid_t pid_gch = fork();
        if (pid_gch < 0) {
            panic("fork failed");
        } else if (pid_gch == 0) {
            if (close(fd[1]) == -1) {
                panic("close failed");
            }
            long long sum = 0;
            int curr = 0;
            while (read(fd[0], &curr, sizeof(curr)) != 0) {
                sum += curr;
            }
            if (close(fd[0]) == -1) {
                panic("close failed");
            }
            dprintf(STDOUT_FILENO, "%lld\n", sum);
            _exit(0);
        } else {
            if (close(fd[0]) == -1) {
                panic("close failed");
            }
            if (close(fd[1]) == -1) {
                panic("close failed");
            }
            wait(NULL);
            _exit(0);
        }
    } else {
        if (close(fd[0]) == -1) {
            panic("close failed");
        }
        int curr = 0;
        while (scanf("%d", &curr) == 1) {
            write(fd[1], &curr, sizeof(curr));
        }
        if (close(fd[1]) == -1) {
            panic("close failed");
        }
        wait(NULL);
        _exit(0);
    }
}
