#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

enum CONSTANTS
{
    ROUND_COEFFICIENT = 10000,
    PERCENT = 100
};

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Error: no initial price given\n");
        exit(1);
    }
    errno = 0;
    char *end_ptr = NULL;
    long double curr = strtold(argv[1], &end_ptr);
    if (errno || *end_ptr || end_ptr == argv[1]) {
        fprintf(stderr, "Error: real number conversion failed\n");
        exit(1);
    }
    long long init = (long long) (curr * ROUND_COEFFICIENT);
    for (int i = 2; i < argc; ++i) {
        errno = 0;
        end_ptr = NULL;
        curr = strtold(argv[i], &end_ptr);
        if (errno || *end_ptr || end_ptr == argv[i]) {
            fprintf(stderr, "Error: real number conversion failed\n");
            exit(1);
        }
        init *= PERCENT;
        init *= (long long) PERCENT * ROUND_COEFFICIENT + (long long) (curr * ROUND_COEFFICIENT);
        init /= (long long) ROUND_COEFFICIENT * PERCENT * PERCENT / 10;
        if (init % 10 >= 5) {
            init += 10;
        }
        init /= 10;
    }
    long long ans_int = init / ROUND_COEFFICIENT;
    long long ans_frac = init % ROUND_COEFFICIENT;
    printf("%lld.%04lld\n", ans_int, ans_frac);
    return 0;
}
