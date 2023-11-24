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
    BASE = 10,
    PROC_CNT = 3
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
handle_line(int id)
{
    char buf[INPUT_SIZE];
    if (read(STDIN_FILENO, buf, sizeof(buf)) != sizeof(buf)) {
        panic("read failed");
    }
    buf[INPUT_SIZE - 1] = '\0';
    long num = strtol(buf, NULL, BASE);
    num *= num;
    dprintf(STDOUT_FILENO, "%d %ld\n", id, num);
    _exit(0);
}

int
main(void)
{
    for (int i = 0; i < PROC_CNT; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            panic("couldn't fork");
        } else if (pid == 0) {
            handle_line(i + 1);
        }
    }
    while (wait(NULL) > 0) {
    }
}
