#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

enum
{
    EXIT_CODE = 42,
    MODE = 0660
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
    pid_t child = fork();
    if (child < 0) {
        panic("fork failed");
    } else if (child == 0) {
        int fd1 = open(argv[2], O_RDONLY);
        if (fd1 == -1) {
            _exit(EXIT_CODE);
        }
        if (dup2(fd1, STDIN_FILENO) == -1) {
            _exit(EXIT_CODE);
        }
        if (close(fd1) == -1) {
            _exit(EXIT_CODE);
        }
        int fd2 = open(argv[3], O_WRONLY | O_CREAT | O_APPEND, MODE);
        if (fd2 == -1) {
            _exit(EXIT_CODE);
        }
        if (dup2(fd2, STDOUT_FILENO) == -1) {
            _exit(EXIT_CODE);
        }
        if (close(fd2) == -1) {
            _exit(EXIT_CODE);
        }
        int fd3 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, MODE);
        if (fd3 == -1) {
            _exit(EXIT_CODE);
        }
        if (dup2(fd3, STDERR_FILENO) == -1) {
            _exit(EXIT_CODE);
        }
        if (close(fd3) == -1) {
            _exit(EXIT_CODE);
        }
        execlp(argv[1], argv[1], NULL);
        _exit(EXIT_CODE);
    } else {
        int status;
        wait(&status);
        printf("%d\n", status);
        return 0;
    }
}
