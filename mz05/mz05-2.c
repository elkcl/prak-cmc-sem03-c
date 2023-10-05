#include <stdio.h>  // fprintf
#include <string.h> // strerror
#include <stdlib.h> // exit()
#include <errno.h>  // errno

const unsigned char TEMPLATE[] = "rwxrwxrwx";

enum
{
    BASE = 8,
    MASK_SIZE = sizeof(TEMPLATE) / sizeof(TEMPLATE[0]) - 1,
    VALID_MASK = ~((1u << MASK_SIZE) - 1)
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
    for (int i = 1; i < argc; ++i) {
        errno = 0;
        char *eptr = NULL;
        long mode = strtol(argv[i], &eptr, BASE);
        if (errno || *eptr || eptr == argv[i] || (mode & VALID_MASK)) {
            panic("incorrect mode format");
        }
        for (int j = 0; j < MASK_SIZE; ++j) {
            if (mode & (1u << (MASK_SIZE - j - 1))) {
                putchar(TEMPLATE[j]);
            } else {
                putchar('-');
            }
        }
        putchar('\n');
    }
}
