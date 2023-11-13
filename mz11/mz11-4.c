#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

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
    int n;
    scanf("%d", &n);
    dprintf(STDOUT_FILENO, "%d", 1);
    for (int i = 2; i <= n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            panic("fork failed");
        } else if (pid != 0) {
            while (wait(NULL) > 0) {}
            _exit(0);
        } else {
            dprintf(STDOUT_FILENO, " %d", i);
        }
    }
    dprintf(STDOUT_FILENO, "\n");
}

