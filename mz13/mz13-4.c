#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

void
handler(int sig)
{
    if (sig == SIGUSR1) {
        raise(SIGKILL);
    }
}

void
fail(void)
{
    kill(0, SIGUSR1);
    while (wait(NULL) > 0) {
    }
    _exit(1);
}

int
main(int argc, char *argv[])
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        _exit(1);
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        _exit(1);
    }
    int dev_null = open("/dev/null", O_WRONLY | O_TRUNC);
    if (dev_null == -1) {
        _exit(1);
    }
    if (dup2(dev_null, STDERR_FILENO) == -1) {
        _exit(1);
    }
    if (close(dev_null) == -1) {
        _exit(1);
    }
    int last_fd[] = {-1, -1};
    for (int i = 1; i < argc; ++i) {
        int curr_fd[2];
        if (i == argc - 1) {
            curr_fd[0] = STDIN_FILENO;
            curr_fd[1] = STDOUT_FILENO;
        } else {
            if (pipe2(curr_fd, O_CLOEXEC) == -1) {
                fail();
            }
        }
        pid_t pid = fork();
        if (pid < 0) {
            fail();
        } else if (pid == 0) {
            if (sigaction(SIGUSR1, &(struct sigaction){.sa_handler = handler}, NULL) == -1) {
                _exit(1);
            }
            if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
                _exit(1);
            }
            if (last_fd[0] != -1) {
                if (dup2(last_fd[0], STDIN_FILENO) == -1) {
                    _exit(1);
                }
            }
            if (dup2(curr_fd[1], STDOUT_FILENO) == -1) {
                _exit(1);
            }
            execlp(argv[i], argv[i], NULL);
            _exit(1);
        }
        if (last_fd[0] != -1) {
            if (close(last_fd[0]) == -1) {
                fail();
            }
            if (close(last_fd[1]) == -1) {
                fail();
            }
        }
        last_fd[0] = curr_fd[0];
        last_fd[1] = curr_fd[1];
    }
    while (wait(NULL) > 0) {
    }
    _exit(0);
}
