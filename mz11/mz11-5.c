#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

enum
{
    INPUT_SIZE = 8,
    BASE = 10
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

int
main(void)
{
    setbuf(stdin, NULL);
    pid_t pid0 = fork();
    if (pid0 < 0) {
        dprintf(STDOUT_FILENO, "%d\n", -1);
        _exit(0);
    } else if (pid0 == 0) {
        while (true) {
            int n;
            if (scanf("%d", &n) != 1) {
                break;
            }
            pid_t pid = fork();
            if (pid < 0) {
                dprintf(STDOUT_FILENO, "%d\n", -1);
                _exit(1);
            } else if (pid != 0) {
                int status = 0;
                bool exit_err_flag = false;
                while (wait(&status) > 0) {
                    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
                        exit_err_flag = true;
                    }
                }
                if (exit_err_flag) {
                    _exit(1);
                }
                dprintf(STDOUT_FILENO, "%d\n", n);
                _exit(0);
            }
        }
    }
    while (wait(NULL) > 0) {
    }
    _exit(0);
}
