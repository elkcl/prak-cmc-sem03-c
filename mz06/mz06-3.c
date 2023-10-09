#include <stddef.h>
struct s1
{
    char f1;
    long long f2;
    char f3;
};

struct s2
{
    long long f2;
    char f1;
    char f3;
};

enum
{
    S1_SIZE = sizeof(struct s1),
    S2_SIZE = sizeof(struct s2)
};

size_t
compactify(void *ptr, size_t size)
{
    if (size == 0 || ptr == NULL) {
        return 0;
    }
    size_t n = size / S1_SIZE;
    struct s1 *p1 = ptr;
    struct s2 *p2 = ptr;
    for (size_t i = 0; i < n; ++i) {
        char f1 = p1[i].f1;
        long long f2 = p1[i].f2;
        char f3 = p1[i].f3;
        p2[i].f1 = f1;
        p2[i].f2 = f2;
        p2[i].f3 = f3;
    }
    return n * S2_SIZE;
}
