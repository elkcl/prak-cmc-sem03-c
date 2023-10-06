#include <stddef.h>   // size_t
#include <stdio.h>    // fprintf
#include <string.h>   // strerror
#include <stdlib.h>   // exit()
#include <errno.h>    // errno
#include <sys/stat.h> // stat()

enum
{
    MIN_ARGC = 2
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

typedef struct
{
    ino_t inode;
    char *path;
} path_t;

int
inode_cmp(const void *x, const void *y)
{
    const path_t *a = x;
    const path_t *b = y;
    if (a->inode == b->inode) {
        return strcmp(b->path, a->path);
    }
    return a->inode - b->inode;
}

int
str_cmp(const void *x, const void *y)
{
    const path_t *a = x;
    const path_t *b = y;
    return strcmp(a->path, b->path);
}

size_t
unique_path(size_t n, path_t arr[])
{
    path_t *last = arr;
    for (size_t i = 1; i < n; ++i) {
        if (arr[i].inode == last->inode) {
            continue;
        }
        ++last;
        *last = arr[i];
    }
    return last - arr + 1;
}

int
main(int argc, char **argv)
{
    if (argc < MIN_ARGC || argv == NULL) {
        return 0;
    }
    path_t *paths = calloc(argc - 1, sizeof(paths[0]));
    if (paths == NULL) {
        panic("couldn't allocate array");
    }
    size_t n = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) {
            continue;
        }
        struct stat info = {};
        if (stat(argv[i], &info) == -1) {
            continue;
        }
        paths[n].inode = info.st_ino;
        paths[n].path = argv[i];
        ++n;
    }
    if (n == 0) {
        free(paths);
        return 0;
    }
    qsort(paths, n, sizeof(paths[0]), inode_cmp);
    size_t new_len = unique_path(n, paths);
    qsort(paths, new_len, sizeof(paths[0]), str_cmp);
    for (size_t i = 0; i < new_len; ++i) {
        puts(paths[i].path);
    }
    free(paths);
}
