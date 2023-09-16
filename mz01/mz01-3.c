#include <stdio.h>

typedef struct GCDRes {
    int x, y, d;
} GCDRes;

GCDRes
gcd(int a, int b)
{
    if (a == 0) {
        return (GCDRes){1, 1, b};
    }
    if (b == 0) {
        return (GCDRes){1, 1, a};
    }
    GCDRes r = gcd(b, a % b);
    int m = a / b;
    return (GCDRes){r.y, r.x - m * r.y, r.d};
}

int
divmod(int c, int a, int n)
{
    GCDRes r = gcd(n, a);
    return ((r.y * c) % n + n) % n;
}

int
main(void)
{
    int n;
    scanf("%d", &n);

    for (int c = 0; c < n; ++c) {
        for (int a = 1; a < n; ++a) {
            printf("%d ", divmod(c, a, n));
        }
        putchar('\n');
    }

    return 0;
}
