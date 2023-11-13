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

void
handle_line(int id)
{
    char buf[INPUT_SIZE];
    if (read(STDIN_FILENO, buf, INPUT_SIZE) != INPUT_SIZE) {
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
    pid_t pid1 = fork();
    if (pid1 < 0) {
        panic("couldn't fork");
    } else if (pid1 == 0) {
        handle_line(1);
    }
    pid_t pid2 = fork();
    if (pid2 < 0) {
        panic("couldn't fork");
    } else if (pid2 == 0) {
        handle_line(2);
    }
    pid_t pid3 = fork();
    if (pid3 < 0) {
        panic("couldn't fork");
    } else if (pid3 == 0) {
        handle_line(3);
    }
    while (wait(NULL) > 0) {
    }
}
