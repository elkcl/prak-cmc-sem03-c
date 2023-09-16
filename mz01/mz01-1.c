#include <stdio.h>

int
main(void)
{
    double x, y;
    scanf("%lf %lf", &x, &y);
    if (x < 2 || x > 5 || y < 1 || y > 7) {
        puts("0");
        return 0;
    }
    if (x >= 3 && x <= 5 && y >= 1 && y <= 3 && (y - 1) < (x - 3)) {
        puts("0");
        return 0;
    }
    puts("1");
    return 0;
}
