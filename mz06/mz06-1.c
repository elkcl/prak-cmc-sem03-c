#include <stddef.h>    // size_t
#include <stdio.h>     // fprintf
#include <string.h>    // strerror
#include <stdlib.h>    // exit()
#include <errno.h>     // errno
#include <sys/types.h> // DIR
#include <sys/stat.h>  // stat
#include <dirent.h>    // opendir()
#include <unistd.h>    // access()

const char ext[] = ".exe";

enum
{
    ARGC = 2,
    DIR_ARG = 1,
    EXT_LEN = sizeof(ext) / sizeof(ext[0]) - 1
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
    size_t dir_len = strlen(argv[DIR_ARG]);
    int cnt = 0;
    struct dirent *curr = NULL;
    while ((curr = readdir(dir)) != NULL) {
        size_t len = strlen(curr->d_name);
        char *full_name = calloc(dir_len + len + 2, sizeof(full_name[0]));
        memcpy(full_name, argv[DIR_ARG], dir_len * sizeof(full_name[0]));
        full_name[dir_len] = '/';
        memcpy(full_name + dir_len + 1, curr->d_name, len * sizeof(full_name[0]));
        full_name[dir_len + len + 2] = '\0';
        if (full_name == NULL) {
            panic("couldn't allocate memory");
        }
        if (strcmp(ext, curr->d_name + len - EXT_LEN) != 0) {
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
