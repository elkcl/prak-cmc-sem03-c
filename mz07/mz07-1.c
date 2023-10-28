#include <ctype.h>  // isspace()
#include <stddef.h> // ssize_t
#include <stdio.h>  // fprintf()
#include <string.h> // strerror
#include <errno.h>  // errno
#include <stdlib.h> // exit()

enum
{
    BASE = 3
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

long long
parse_num(const char *str, ssize_t n)
{
    long long ans = 0;
    for (ssize_t i = 0; i < n; ++i) {
        if (str[i] == '1') {
            if (ans < 0) {
                ++ans;
                if (__builtin_mul_overflow(ans, BASE, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
                if (__builtin_sub_overflow(ans, BASE - 1, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
            } else {
                if (__builtin_mul_overflow(ans, BASE, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
                if (__builtin_add_overflow(ans, 1, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
            }
        } else if (str[i] == 'a') {
            if (ans > 0) {
                --ans;
                if (__builtin_mul_overflow(ans, BASE, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
                if (__builtin_add_overflow(ans, BASE - 1, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
            } else {
                if (__builtin_mul_overflow(ans, BASE, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
                if (__builtin_sub_overflow(ans, 1, &ans)) {
                    errno = ERANGE;
                    return -1;
                }
            }
        } else if (str[i] == '0') {
            if (__builtin_mul_overflow(ans, BASE, &ans)) {
                errno = ERANGE;
                return -1;
            }
        } else {
            errno = EINVAL;
            return -1;
        }
    }
    return ans;
}

int
main(void)
{
    char *line = NULL;
    size_t buf_size = 0;
    ssize_t len = 0;
    while ((len = getline(&line, &buf_size, stdin)) != -1) {
        char *from = line;
        char *to;
        while (*from != '\0' && isspace(*from)) {
            ++from;
        }
        to = from;
        while (*from != '\0') {
            while (*to != '\0' && !isspace(*to)) {
                ++to;
            }
            errno = 0;
            long long curr = parse_num(from, to - from);
            if (curr == -1 && errno) {
                if (errno == ERANGE) {
                    puts("18446744073709551616");
                }
            } else {
                printf("%lld\n", curr);
            }
            while (*to != '\0' && isspace(*to)) {
                ++to;
            }
            from = to;
        }
    }
    free(line);
}
