#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

enum
{
    TIME_CNT = 3,
    MON_OFFSET = 1,
    YEAR_OFFSET = 1900
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
            pid_t pid_ggch = fork();
            if (pid_ggch < 0) {
                panic("fork failed");
            } else if (pid_ggch == 0) {
                if (close(fd[0]) == -1) {
                    panic("close failed");
                }
                time_t now = time(NULL);
                for (int i = 0; i < TIME_CNT; ++i) {
                    write(fd[1], &now, sizeof(now));
                }
                if (close(fd[1]) == -1) {
                    panic("close failed");
                }
                _exit(0);
            } else {
                if (close(fd[1]) == -1) {
                    panic("close failed");
                }
                wait(NULL);
                time_t now;
                read(fd[0], &now, sizeof(now));
                if (close(fd[0]) == -1) {
                    panic("close failed");
                }
                struct tm *now_s = localtime(&now);
                dprintf(STDOUT_FILENO, "D:%02d\n", now_s->tm_mday);
                _exit(0);
            }
        } else {
            if (close(fd[1]) == -1) {
                panic("close failed");
            }
            wait(NULL);
            time_t now;
            read(fd[0], &now, sizeof(now));
            if (close(fd[0]) == -1) {
                panic("close failed");
            }
            struct tm *now_s = localtime(&now);
            dprintf(STDOUT_FILENO, "M:%02d\n", now_s->tm_mon + MON_OFFSET);
            _exit(0);
        }
    } else {
        if (close(fd[1]) == -1) {
            panic("close failed");
        }
        wait(NULL);
        time_t now;
        read(fd[0], &now, sizeof(now));
        if (close(fd[0]) == -1) {
            panic("close failed");
        }
        struct tm *now_s = localtime(&now);
        dprintf(STDOUT_FILENO, "Y:%04d\n", now_s->tm_year + YEAR_OFFSET);
        _exit(0);
    }
}
