#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
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
    int ans = 0;
    int status = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == 'p') {
            pid_t pid = fork();
            if (pid < 0) {
                panic("fork failed");
            } else if (pid == 0) {
                execlp(argv[i] + 1, argv[i] + 1, NULL);
                panic("exec failed");
            }
        } else {
            errno = 0;
            while (wait(&status) > 0) {
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    ++ans;
                }
            }
            if (errno && errno != ECHILD) {
                panic("wait failed");
            }
            pid_t pid = fork();
            if (pid < 0) {
                panic("fork failed");
            } else if (pid == 0) {
                execlp(argv[i] + 1, argv[i] + 1, NULL);
                panic("exec failed");
            } else {
                errno = 0;
                wait(&status);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    ++ans;
                }
                if (errno && errno != ECHILD) {
                    panic("wait failed");
                }
            }
        }
    }
    errno = 0;
    while (wait(&status) > 0) {
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            ++ans;
        }
    }
    if (errno && errno != ECHILD) {
        panic("wait failed");
    }
    printf("%d\n", ans);
    return 0;
}
