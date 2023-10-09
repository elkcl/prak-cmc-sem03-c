#include <stddef.h>    // size_t
#include <stdio.h>     // fprintf
#include <string.h>    // strerror
#include <stdlib.h>    // exit()
#include <errno.h>     // errno
#include <sys/types.h> // DIR
#include <sys/stat.h>  // stat
#include <dirent.h>    // opendir()
#include <unistd.h>    // access()

enum
{
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
}

