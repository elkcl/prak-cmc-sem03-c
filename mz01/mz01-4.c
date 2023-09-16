#include <stdio.h>
#include <stdlib.h>

int
cmp(const void *av, const void *bv)
{
    int a = *((int *) av);
    int b = *((int *) bv);
    if (a % 2 == 0) {
        if (b % 2 == 0) {
            if (a < b) {
                return -1;
            } else if (a == b) {
                return 0;
            } else {
                return 1;
            }
        } else {
            return -1;
        }
    } else {
        if (b % 2 == 0) {
            return 1;
        } else {
            if (a < b) {
                return 1;
            } else if (a == b) {
                return 0;
            } else {
                return -1;
            }
        }
    }
}

void
sort_even_odd(size_t count, int *data)
{
    qsort(data, count, sizeof(data[0]), cmp);
}

// int main(void) {
//     int test[] = {5, 2, 3, 3, 4, 10, 7, 8, 0};
//     sort_even_odd(sizeof(test) / sizeof(test[0]), test);
//     for (int i = 0 ; i < sizeof(test) / sizeof(test[0]); ++i) {
//         printf("%d ", test[i]);
//     }
//     putchar('\n');
//     return 0;
// }
