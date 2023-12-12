#include <bits/wait.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum
{
    ARGC_MIN = 2,
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
main(int argc, char *argv[])
{
    if (argc < ARGC_MIN) {
        panic("incorrect input format");
    }
    errno = 0;
    char *eptr = NULL;
    int n = strtol(argv[1], &eptr, BASE);
    if (errno || *eptr || eptr == argv[1]) {
        panic("incorrect input format");
    }
    int par_cnt = (argc - 2 < n) ? (argc - 2) : n;
    int ans = 0;
    char buf[PATH_MAX];
    for (int i = 0; i < par_cnt; ++i) {
        if (fork() == 0) {
            FILE *file = fopen(argv[i + 2], "r");
            if (file == NULL) {
                _exit(1);
            }
            if (fgets(buf, PATH_MAX, file) == NULL) {
                _exit(1);
            }
            buf[strlen(buf) - 1] = '\0';
            if (fclose(file) < 0) {
                _exit(1);
            }
            execlp(buf, buf, NULL);
            _exit(1);
        }
    }
    int status;
    while (wait(&status) > 0) {
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            ++ans;
        }
    }
    for (int i = par_cnt; i + 2 < argc; ++i) {
        if (fork() == 0) {
            FILE *file = fopen(argv[i + 2], "r");
            if (file == NULL) {
                _exit(1);
            }
            if (fgets(buf, PATH_MAX, file) == NULL) {
                _exit(1);
            }
            buf[strlen(buf) - 1] = '\0';
            if (fclose(file) < 0) {
                _exit(1);
            }
            execlp(buf, buf, NULL);
            _exit(1);
        }
        if (wait(&status) > 0) {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                ++ans;
            }
        }
    }
}
