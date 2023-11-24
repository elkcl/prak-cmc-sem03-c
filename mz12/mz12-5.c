#include <linux/limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

const char PROG[] = "#include <stdio.h>\n"
                    "const char summon[] = \"summon\";\n"
                    "const char reject[] = \"reject\";\n"
                    "const char disqualify[] = \"disqualify\";\n"
                    "int main(void){\n"
                    "int x;\n"
                    "while (scanf(\"%%d\", &x) == 1) {\n"
                    "puts(%s);}\n"
                    "remove(\"%s\");}\n";

const char SCRIPT[] = "#!/bin/bash\n"
                      "gcc -O2 --std=gnu2x -o '%s' '%s' 1>/dev/null 2>/dev/null\n"
                      "rm '%s'\n"
                      "rm '%s'\n"
                      "exec '%s'\n";

enum
{
    ARGC = 2,
    SCRIPT_MODE = 0700,
    PROG_MODE = 0600
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
    if (argc != ARGC || argv == NULL || argv[1] == NULL) {
        panic("incorrect input format");
    }
    char *tmpdir = getenv("XDG_RUNTIME_DIR");
    if (tmpdir == NULL) {
        tmpdir = getenv("TMPDIR");
        if (tmpdir == NULL) {
            tmpdir = "/tmp";
        }
    }
    char exe_path[PATH_MAX];
    if (snprintf(exe_path, PATH_MAX, "%s/elk%d", tmpdir, getpid()) >= PATH_MAX) {
        panic("executable name too long");
    }
    char prog_path[PATH_MAX];
    if (snprintf(prog_path, PATH_MAX, "%s.c", exe_path) >= PATH_MAX) {
        panic("C source name too long");
    }
    char script_path[PATH_MAX];
    if (snprintf(script_path, PATH_MAX, "%s.sh", exe_path) >= PATH_MAX) {
        panic("script name too long");
    }
    int script = open(script_path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, SCRIPT_MODE);
    if (script == -1) {
        panic("opening script file failed");
    }
    if (dprintf(script, SCRIPT, exe_path, prog_path, prog_path, script_path, exe_path) < 0) {
        panic("writing to script file failed");
    }
    if (close(script) == -1) {
        panic("closing script file failed");
    }
    int prog = open(prog_path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, PROG_MODE);
    if (prog == -1) {
        panic("opening C source file failed");
    }
    if (dprintf(prog, PROG, argv[1], exe_path) < 0) {
        panic("writing to C source file failed");
    }
    if (close(script) == -1) {
        panic("closing C source file failed");
    }
    execlp(script_path, script_path, NULL);
    panic("execlp failed");
}
