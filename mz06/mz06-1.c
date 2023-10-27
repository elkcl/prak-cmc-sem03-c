#include <linux/limits.h>
#include <stddef.h>    // size_t
#include <stdio.h>     // fprintf
#include <string.h>    // strerror
#include <stdlib.h>    // exit()
#include <errno.h>     // errno
#include <sys/types.h> // DIR
#include <sys/stat.h>  // stat
#include <dirent.h>    // opendir()
#include <unistd.h>    // access()
#include <stdbool.h>   // bool

const char EXT[] = ".exe";

enum
{
    ARGC = 2,
    DIR_ARG = 1,
    EXT_LEN = sizeof(EXT) / sizeof(EXT[0]) - 1
};

void
panic(char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    exit(1);
}

bool
check_suffix(const char *str, size_t str_len, const char *suff, size_t suff_len)
{
    if (suff_len > str_len) {
        return false;
    }
    return strcmp(str + str_len - suff_len, suff) == 0;
}

bool
check_file(const char *name, size_t len)
{
    if (!check_suffix(name, len, EXT, EXT_LEN)) {
        return false;
    }
    if (access(name, X_OK) != 0) {
        return false;
    }
    struct stat info = {};
    if (stat(name, &info) == -1) {
        return false;
    }
    if (!S_ISREG(info.st_mode)) {
        return false;
    }
    return true;
}

int
main(int argc, char **argv)
{
    if (argc != ARGC || argv == NULL || argv[DIR_ARG] == NULL) {
        panic("incorrect argument format");
    }
    DIR *dir = opendir(argv[DIR_ARG]);
    if (dir == NULL) {
        panic("couldn't open directory");
    }
    int cnt = 0;
    struct dirent *curr = NULL;
    char full_name[PATH_MAX];
    while ((curr = readdir(dir)) != NULL) {
        size_t full_len = snprintf(full_name, PATH_MAX, "%s/%s", argv[DIR_ARG], curr->d_name);
        if (full_len >= PATH_MAX) {
            panic("path too long");
        }
        if (check_file(full_name, full_len)) {
            ++cnt;
        }
    }
    printf("%d\n", cnt);

    if (closedir(dir) == -1) {
        panic("couldn't close directory");
    }
}
