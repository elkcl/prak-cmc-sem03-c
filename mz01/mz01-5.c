#include <stdio.h>

void
permutations(int n, int c, char *curr, int *used)
{
    if (c >= n) {
        puts(curr);
        return;
    }
    for (int i = 1; i <= n; ++i) {
        if (used[i - 1]) {
            continue;
        }
        used[i - 1] = 1;
        curr[c] = i + '0';
        curr[c + 1] = '\0';
        permutations(n, c + 1, curr, used);
        used[i - 1] = 0;
    }
}

int
main(void)
{
    int n;
    scanf("%d", &n);
    char curr[12] = {};
    int used[12] = {};
    permutations(n, 0, curr, used);
}
