#include <stdint.h>   // uintmax_t
#include <stdio.h>    // printf
#include <sys/stat.h> // lstat()

enum
{
    SIZE_KIB_MASK = 0x3FF
};

int
main(int argc, char **argv)
{
    uintmax_t ans = 0;
    for (int i = 1; i < argc; ++i) {
        struct stat info = {};
        if (lstat(argv[i], &info) == -1) {
            continue;
        }
        if (S_ISREG(info.st_mode) && info.st_nlink == 1 && (info.st_size & SIZE_KIB_MASK) == 0) {
            ans += info.st_size;
        }
    }
    printf("%ju\n", ans);
}
