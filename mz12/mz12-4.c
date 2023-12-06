#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

const char CODE1[] = "#!/usr/bin/python3\n"
                     "import os\n"
                     "print(";
const char CODE2[] = ")\n"
                     "os.remove('";
const char CODE3[] = "')\n";

enum
{
    MODE = 0700,
    RAND_CNT = 20,
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
    char *tmpdir = getenv("XDG_RUNTIME_DIR");
    if (tmpdir == NULL) {
        tmpdir = getenv("TMPDIR");
        if (tmpdir == NULL) {
            tmpdir = "/tmp";
        }
    }
    FILE *rd = fopen("/dev/urandom", "rb");
    if (rd == NULL) {
        panic("fopen failed");
    }
    unsigned char buf[] = "01234567890123456789.py";
    if (fread(buf, sizeof(buf[0]), RAND_CNT, rd) != RAND_CNT) {
        panic("fread failed");
    }
    for (int i = 0; i < 20; ++i) {
        buf[i] = buf[i] % 10 + '0';
    }
    if (fclose(rd) < 0) {
        panic("fclose failed");
    }
    char path[PATH_MAX];
    size_t path_len = 0;
    if ((path_len = snprintf(path, PATH_MAX, "%s/%s", tmpdir, buf)) >= PATH_MAX) {
        panic("path too long");
    }
    FILE *file = fopen(path, "w");
    if (fchmod(fileno(file), MODE) == -1) {
        panic("chmod failed");
    }
    if (fputs(CODE1, file) < 0) {
        panic("fputs failed");
    }
    for (int i = 1; i < argc; ++i) {
        if (fputs(argv[i], file) < 0) {
            panic("fputs failed");
        }
        if (i != argc - 1) {
            if (fputc('*', file) < 0) {
                panic("fputc failed");
            }
        }
    }
    if (fputs(CODE2, file) < 0) {
        panic("fputs failed");
    }
    if (fputs(path, file) < 0) {
        panic("fputs failed");
    }
    if (fputs(CODE3, file) < 0) {
        panic("fputs failed");
    }
    if (fclose(file) < 0) {
        panic("close failed");
    }
    execlp(path, path, NULL);
    panic("exec failed");
}
