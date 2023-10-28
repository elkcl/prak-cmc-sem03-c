#include <stdio.h>  // fprintf()
#include <string.h> // strerror()
#include <errno.h>  // errno
#include <stdlib.h> // exit()
#include <time.h>   // mktime()

enum
{
    YEAR_OFFSET = 1900,
    YEAR_MAX_DAY = 366,
    HOUR = 12,
    MDAY_MOD = 3,
    THU_CNT1 = 2,
    THU_CNT2 = 4,
    WEEK = 7
};

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
    year -= YEAR_OFFSET;
    struct tm curr = {};
    int day = 0;
    do {
        ++day;
        memset(&curr, 0, sizeof(curr));
        curr.tm_isdst = -1;
        curr.tm_hour = HOUR;
        curr.tm_year = year;
        curr.tm_mon = 0;
        curr.tm_mday = day;
        errno = 0;
        if (mktime(&curr) == -1 && errno) {
            panic("mktime failed");
        }
    } while (curr.tm_wday != 4);
    int last_mon = 0;
    int thu_cnt = 0;
    for (; day <= YEAR_MAX_DAY; day += WEEK) {
        memset(&curr, 0, sizeof(curr));
        curr.tm_isdst = -1;
        curr.tm_hour = HOUR;
        curr.tm_year = year;
        curr.tm_mon = 0;
        curr.tm_mday = day;
        errno = 0;
        if (mktime(&curr) == -1 && errno) {
            panic("mktime failed");
        }
        if (curr.tm_year != year) {
            break;
        }
        if (curr.tm_mon != last_mon) {
            thu_cnt = 0;
            last_mon = curr.tm_mon;
        }
        ++thu_cnt;
        if (curr.tm_mday % MDAY_MOD != 0 && (thu_cnt == THU_CNT1 || thu_cnt == THU_CNT2)) {
            printf("%d %d\n", curr.tm_mon + 1, curr.tm_mday);
        }
    }
}
