#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

volatile sig_atomic_t sig_cnt = 0;

void
handler(int sig)
{
    if (sig == SIGHUP) {
        sig_cnt = 1;
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
    sigaddset(&set, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        panic("sigprocmask failed");
    }
    sigemptyset(&set);
    if (sigaction(SIGHUP, &(const struct sigaction){.sa_handler = handler}, NULL) == -1) {
        panic("sigaction failed");
    }
    dprintf(STDOUT_FILENO, "%d\n", getpid());
    int cnt = 0;
    while (cnt < 5) {
        sigsuspend(&set);
        if (sig_cnt != 1) {
            continue;
        }
        sig_cnt = 0;
        dprintf(STDOUT_FILENO, "%d\n", cnt);
        ++cnt;
    }
    while (sig_cnt != 1) {
        sigsuspend(&set);
    }
}
