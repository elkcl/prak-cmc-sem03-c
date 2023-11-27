#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

enum
{
    ARGC = 6,
    CMD1_ARG = 1,
    CMD2_ARG = 2,
    CMD3_ARG = 3,
    FILE1_ARG = 4,
    FILE2_ARG = 5,
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
    if (argc != ARGC) {
        panic("incorrect input format");
    }
    int fd[2];
    if (pipe2(fd, O_CLOEXEC) == -1) {
        panic("pipe failed");
    }
    pid_t pid1 = fork();
    if (pid1 < 0) {
        panic("fork failed");
    } else if (pid1 == 0) {
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            panic("dup2 failed");
        }
        if (close(fd[0]) == -1) {
            panic("close failed");
        }
        if (close(fd[1]) == -1) {
            panic("close failed");
        }
        pid_t cmd1 = fork();
        if (cmd1 < 0) {
            panic("fork failed");
        } else if (cmd1 == 0) {
            int file1 = open(argv[FILE1_ARG], O_RDONLY | O_CLOEXEC);
            if (file1 == -1) {
                panic("open failed");
            }
            if (dup2(file1, STDIN_FILENO) == -1) {
                panic("dup2 failed");
            }
            execlp(argv[CMD1_ARG], argv[CMD1_ARG], NULL);
            panic("exec failed");
        }
        int status;
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            pid_t cmd2 = fork();
            if (cmd2 < 0) {
                panic("fork failed");
            } else if (cmd2 == 0) {
                execlp(argv[CMD2_ARG], argv[CMD2_ARG], NULL);
                panic("exec failed");
            }
            wait(NULL);
        }
        _exit(0);
    } else {
        pid_t cmd3 = fork();
        if (cmd3 < 0) {
            panic("fork failed");
        } else if (cmd3 == 0) {
            if (dup2(fd[0], STDIN_FILENO) == -1) {
                panic("dup2 failed");
            }
            int file2 = open(argv[FILE2_ARG], O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, MODE);
            if (file2 == -1) {
                panic("open failed");
            }
            if (dup2(file2, STDOUT_FILENO) == -1) {
                panic("dup2 failed");
            }
            execlp(argv[CMD3_ARG], argv[CMD3_ARG], NULL);
            panic("exec failed");
        }
        if (close(fd[0]) == -1) {
            panic("close failed");
        }
        if (close(fd[1]) == -1) {
            panic("close failed");
        }
        while (wait(NULL) > 0) {
        }
        _exit(0);
    }
}
