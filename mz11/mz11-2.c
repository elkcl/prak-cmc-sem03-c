#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>


void
panic(char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    _exit(1);
}

int
main(void)
{
   pid_t pid1 = fork();
    if (pid1 < 0) {
        panic("couldn't fork");
    } else if (pid1 == 0) {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            panic("couldn't fork");
        } else if (pid2 == 0) {
            dprintf(STDOUT_FILENO, "%d", 3);
            _exit(0);
        }
        wait(NULL);
        dprintf(STDOUT_FILENO, " %d ", 2);
        _exit(0);
    }
    wait(NULL);
    dprintf(STDOUT_FILENO, "%d\n", 1);
}
