#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

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
    long long curr = 1;
    bool invalid_curr = false;
    for (ssize_t i = n - 1; i >= 0; --i) {
        if (str[i] == '1') {
            if (invalid_curr) {
                errno = ERANGE;
                return -1;
            }
            bool over = __builtin_saddll_overflow(ans, curr, &ans);
            if (over) {
                errno = ERANGE;
                return -1;
            }
        } else if (str[i] == 'a') {
            if (invalid_curr) {
                errno = ERANGE;
                return -1;
            }
            bool over = __builtin_ssubll_overflow(ans, curr, &ans);
            if (over) {
                errno = ERANGE;
                return -1;
            }
        } else if (str[i] != '0') {
            errno = EINVAL;
            return -1;
        }
        bool over = __builtin_smulll_overflow(curr, 3, &curr);
        if (over) {
            invalid_curr = true;
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
        char *to = line;
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
