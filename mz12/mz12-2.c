#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

bool
launch_proc(const char *cmd)
{
    pid_t pid = fork();
    if (pid < 0) {
        _exit(1);
    } else if (pid == 0) {
        execlp(cmd, cmd, NULL);
        _exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

int
main(int argc, char *argv[])
{
    if ((launch_proc(argv[1]) || launch_proc(argv[2])) && launch_proc(argv[3])) {
        return 0;
    } else {
        return 1;
    }
}
