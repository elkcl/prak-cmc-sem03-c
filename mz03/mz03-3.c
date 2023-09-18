#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

enum CONSTANTS
{
    ROUND_COEFFICIENT = 10000
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
    long double init = strtold(argv[1], &end_ptr);
    if (errno || *end_ptr || end_ptr == argv[1]) {
        fprintf(stderr, "Error: real number conversion failed\n");
        exit(1);
    }
    for (int i = 2; i < argc; ++i) {
        errno = 0;
        end_ptr = NULL;
        long double curr = strtold(argv[i], &end_ptr);
        if (errno || *end_ptr || end_ptr == argv[i]) {
            fprintf(stderr, "Error: real number conversion failed\n");
            exit(1);
        }
        init *= 1 + 0.01 * curr;
        init = roundl(init * ROUND_COEFFICIENT) / ROUND_COEFFICIENT;
    }
    long long ans = llroundl(init * ROUND_COEFFICIENT);
    long long ans_int = ans / ROUND_COEFFICIENT;
    long long ans_frac = ans % ROUND_COEFFICIENT;
    printf("%Ld.%Ld\n", ans_int, ans_frac);
    return 0;
}
