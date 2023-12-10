#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

enum
{
    ADD = 0,
    MUL = 1
};

volatile sig_atomic_t mode = ADD;

void
handler(int sig)
{
    if (sig == SIGINT) {
        mode = ADD;
    } else if (sig == SIGQUIT) {
        mode = MUL;
    }
}

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
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        panic("sigprocmask failed");
    }
    if (sigaction(SIGINT, &(const struct sigaction){.sa_handler = handler, .sa_mask = set, .sa_flags = SA_RESTART},
                  NULL) == -1) {
        panic("sigaction failed");
    }
    if (sigaction(SIGQUIT, &(const struct sigaction){.sa_handler = handler, .sa_mask = set, .sa_flags = SA_RESTART},
                  NULL) == -1) {
        panic("sigaction failed");
    }
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
        panic("sigprocmask failed");
    }
    dprintf(STDOUT_FILENO, "%d\n", getpid());
    setbuf(stdin, NULL);
    int ans = 0, curr = 0;
    long long temp = 0;
    while (scanf("%d", &curr) == 1) {
        if (mode == ADD) {
            temp = 1ll * ans + curr;
            ans = temp;
        } else if (mode == MUL) {
            temp = 1ll * ans * curr;
            ans = temp;
        }
        dprintf(STDOUT_FILENO, "%d\n", ans);
    }
}
