#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <time.h>

void
panic(char *err)
{
    if (errno) {
        fprintf(stderr, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        fprintf(stderr, "Error: %s\n", err);
    }
    exit(1);
}

int
main(void)
{
    int year = 0;
    scanf("%d", &year);
    year -= 1900;
    struct tm curr = {};
    int last_mon = 0;
    int thu_cnt = 0;
    for (int day = 1; day <= 366; ++day) {
        memset(&curr, 0, sizeof(curr));
        curr.tm_isdst = -1;
        curr.tm_hour = 12;
        curr.tm_year = year;
        curr.tm_mon = 0;
        curr.tm_mday = day;
        mktime(&curr);
        if (curr.tm_year != year) {
            break;
        }
        if (curr.tm_mon != last_mon) {
            thu_cnt = 0;
            last_mon = curr.tm_mon;
        }
        if (curr.tm_wday == 4) {
            ++thu_cnt;
        }
        if (curr.tm_mday % 3 != 0 && curr.tm_wday == 4 && (thu_cnt == 2 || thu_cnt == 4)) {
            printf("%d %d\n", curr.tm_mon + 1, curr.tm_mday);
        }
    }
}
