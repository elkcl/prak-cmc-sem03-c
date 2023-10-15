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
    while ((curr = readdir(dir)) != NULL) {
        size_t len = strlen(curr->d_name);
        char *full_name = NULL;
        size_t full_len = asprintf(&full_name, "%s/%s", argv[DIR_ARG], curr->d_name);
        if (full_len == -1) {
            panic("couldn't allocate memory");
        }
        if (!check_suffix(curr->d_name, len, EXT, EXT_LEN)) {
            continue;
        }
        if (access(full_name, X_OK) != 0) {
            continue;
        }
        struct stat info = {};
        if (stat(full_name, &info) == -1) {
            continue;
        }
        if (!S_ISREG(info.st_mode)) {
            continue;
        }
        ++cnt;
        free(full_name);
    }
    printf("%d\n", cnt);

    if (closedir(dir) == -1) {
        panic("couldn't close directory");
    }
}
