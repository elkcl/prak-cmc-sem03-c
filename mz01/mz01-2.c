#include <stdio.h>

int
main(void)
{
    const unsigned int XOR_MASK = 1u << 3;
    const unsigned int AND_MASK = ~(1u << 2);

    int c;
    while ((c = getchar()) != EOF) {
        unsigned int code;
        if ('0' <= c && c <= '9') {
            code = 1 + c - '0';
        } else if ('a' <= c && c <= 'z') {
            code = 11 + c - 'a';
        } else if ('A' <= c && c <= 'Z') {
            code = 37 + c - 'A';
        } else {
            continue;
        }
        code ^= XOR_MASK;
        code &= AND_MASK;
        if (code == 0) {
            putchar('@');
        } else if (code == 63) {
            putchar('#');
        } else if (code <= 10) {
            putchar(code + '0' - 1);
        } else if (code <= 36) {
            putchar(code + 'a' - 11);
        } else {
            putchar(code + 'A' - 37);
        }
    }
    putchar('\n');
}
