#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum CONSTANTS
{
    MAX_SIZE = 130
};

int
main(void)
{
    char s1[MAX_SIZE];
    char s2[MAX_SIZE];
    char s3[MAX_SIZE];
    if (!fgets(s1, MAX_SIZE, stdin)) {
        fputs("Input format error\n", stderr);
        exit(1);
    }
    if (!fgets(s2, MAX_SIZE, stdin)) {
        fputs("Input format error\n", stderr);
        exit(1);
    }
    if (!fgets(s3, MAX_SIZE, stdin)) {
        fputs("Input format error\n", stderr);
        exit(1);
    }
    int n1 = strlen(s1) - 1;
    int n2 = strlen(s2) - 1;
    int n3 = strlen(s3) - 1;
    s1[n1] = '\0';
    s2[n2] = '\0';
    s3[n3] = '\0';
    printf("[Host:%s,Login:%s,Password:%s]\n", s1, s2, s3);
}
