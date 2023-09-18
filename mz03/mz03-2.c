#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

int
main(int argc, char **argv)
{
    long long sum_pos = 0;
    long long sum_neg = 0;
    for (int i = 1; i < argc; ++i) {
        char *str_end = NULL;
        errno = 0;
        long long curr = strtoll(argv[i], &str_end, 10);
        if (errno || (curr == 0 && argv[i] == str_end)) {
            fprintf(stderr, "Error: conversion to integer failed\n");
            exit(1);
        }
        if (curr > 0) {
            bool check = __builtin_add_overflow(sum_pos, curr, &sum_pos);
            if (check) {
                fprintf(stderr, "Error: integer overflow\n");
                exit(1);
            }
        } else if (curr < 0) {
            bool check = __builtin_add_overflow(sum_neg, curr, &sum_neg);
            if (check) {
                fprintf(stderr, "Error: integer overflow\n");
                exit(1);
            }
        }
    }
    printf("%Ld\n%Ld\n", sum_pos, sum_neg);
    return 0;
}
