#include <stddef.h>    // size_t
#include <stdio.h>     // fprintf
#include <string.h>    // strerror
#include <stdlib.h>    // exit()
#include <errno.h>     // errno
#include <sys/types.h> // DIR
#include <sys/stat.h>  // stat
#include <dirent.h>    // opendir()
#include <unistd.h>    // access()

const char CURR[] = ".";
const char PREV[] = "..";

enum
{
    ARGC = 3,
    PATH_ARG = 1,
    Z_ARG = 2,
    BASE = 10,
    MAX_REC_DEPTH = 4
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

void
print_paths(size_t init_dir_len, const char *path, off_t max_sz, int depth)
{
    if (depth > MAX_REC_DEPTH) {
        return;
    }
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }
    struct dirent *curr = NULL;
    while ((curr = readdir(dir)) != NULL) {
        if (strcmp(curr->d_name, CURR) == 0) {
            continue;
        }
        if (strcmp(curr->d_name, PREV) == 0) {
            continue;
        }
        char *full_name = NULL;
        int full_len = asprintf(&full_name, "%s/%s", path, curr->d_name);
        if (full_len == -1) {
            panic("couldn't allocate memory");
        }
        struct stat info = {};
        if (lstat(full_name, &info) == -1) {
            free(full_name);
            continue;
        }
        if (S_ISDIR(info.st_mode)) {
            print_paths(init_dir_len, full_name, max_sz, depth + 1);
        } else if (S_ISREG(info.st_mode)) {
            if (access(full_name, R_OK) != 0) {
                free(full_name);
                continue;
            }
            if (info.st_size > max_sz) {
                free(full_name);
                continue;
            }
            puts(full_name + init_dir_len + 1);
        }
        free(full_name);
    }
    closedir(dir);
}

int
main(int argc, char **argv)
{
    if (argc != ARGC || argv == NULL || argv[PATH_ARG] == NULL || argv[Z_ARG] == NULL) {
        panic("incorrect argument format");
    }
    errno = 0;
    char *eptr = NULL;
    long max_sz = strtol(argv[Z_ARG], &eptr, BASE);
    if (errno || *eptr || eptr == argv[Z_ARG]) {
        panic("incorrect argument format");
    }
    size_t init_dir_len = strlen(argv[PATH_ARG]);
    print_paths(init_dir_len, argv[PATH_ARG], max_sz, 1);
}
