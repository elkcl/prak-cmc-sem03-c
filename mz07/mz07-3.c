#include <stdio.h>    // fprintf()
#include <string.h>   // strerror()
#include <errno.h>    // errno
#include <stdlib.h>   // exit()
#include <sys/stat.h> // stat()
#include <dirent.h>   // dirent
#include <unistd.h>   // access()

void
panic(const char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    exit(1);
}

enum
{
    ARGC = 3,
    DIR1_ARG = 1,
    DIR2_ARG = 2
};

int
main(int argc, char **argv)
{
    if (argc != ARGC || argv == NULL || argv[DIR1_ARG] == NULL || argv[DIR2_ARG] == NULL) {
        panic("incorrect argument format");
    }
    DIR *dir1 = opendir(argv[DIR1_ARG]);
    if (dir1 == NULL) {
        panic("couldn't open dir1");
    }
    char buf[PATH_MAX];
    struct dirent *curr;
    size_t ans = 0;
    while ((curr = readdir(dir1)) != NULL) {
        if (snprintf(buf, PATH_MAX, "%s/%s", argv[DIR1_ARG], curr->d_name) >= PATH_MAX) {
            closedir(dir1);
            panic("path too long");
        }
        struct stat info;
        if (lstat(buf, &info) != -1 && S_ISREG(info.st_mode) && access(buf, W_OK) == 0) {
            ino_t curr_ino = info.st_ino;
            dev_t curr_dev = info.st_dev;
            if (snprintf(buf, PATH_MAX, "%s/%s", argv[DIR2_ARG], curr->d_name) >= PATH_MAX) {
                closedir(dir1);
                panic("path too long");
            }
            if (stat(buf, &info) != -1 && info.st_ino == curr_ino && info.st_dev == curr_dev) {
                ans += info.st_size;
            }
        }
    }
    printf("%zu\n", ans);
    if (closedir(dir1) == -1) {
        panic("couldn't close dir1");
    }
}
