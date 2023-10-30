#include <stdio.h>  // fprintf()
#include <string.h> // strerror()
#include <errno.h>  // errno
#include <stdlib.h> // exit()
#include <time.h>   // mktime()

enum
{
    FULL_MOON_DIFF = (time_t) 29 * 24 * 3600 + 12 * 3600 + 44 * 60,
    YEAR_OFFSET = 1900,
    MONTH_OFFSET = 1,
    MONDAY = 1,
    YDAY_THRESHOLD = 257,
    CORRECT_DAY_OFFSET = 3 * 7,
    OUT_BUF_SIZE = 11
};

const struct tm FULL_MOON = {.tm_sec = 0,
                             .tm_min = 44,
                             .tm_hour = 11,
                             .tm_mday = 26,
                             .tm_mon = 5 - MONTH_OFFSET,
                             .tm_year = 2021 - YEAR_OFFSET,
                             .tm_isdst = -1};

void
panic(const char *err)
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
    int year;
    if (scanf("%d", &year) != 1) {
        panic("incorrect input format");
    }
    year -= YEAR_OFFSET;
    struct tm curr_moon = FULL_MOON;
    errno = 0;
    time_t curr_moon_sec = mktime(&curr_moon);
    if (curr_moon_sec == -1 && errno) {
        panic("couldn't mktime");
    }
    struct tm curr;
    memset(&curr, 0, sizeof(curr));
    curr.tm_mday = YDAY_THRESHOLD;
    curr.tm_year = year;
    curr.tm_isdst = -1;
    errno = 0;
    time_t curr_sec = mktime(&curr);
    if (curr_sec == -1 && errno) {
        panic("couldn't mktime");
    }
    if (curr_moon_sec > curr_sec) {
        curr_moon_sec = curr_sec + (curr_moon_sec - curr_sec) % FULL_MOON_DIFF;
    } else if (curr_moon_sec < curr_sec) {
        curr_moon_sec = curr_sec + FULL_MOON_DIFF - (curr_sec - curr_moon_sec) % FULL_MOON_DIFF;
    }
    const struct tm *new_full_moon = gmtime(&curr_moon_sec);
    if (new_full_moon == NULL) {
        panic("couldn't gmtime");
    }
    memcpy(&curr_moon, new_full_moon, sizeof(curr_moon));
    ++curr_moon.tm_mday;
    curr_moon.tm_sec = 0;
    curr_moon.tm_min = 0;
    curr_moon.tm_hour = 0;
    curr_moon.tm_isdst = -1;
    errno = 0;
    curr_moon_sec = mktime(&curr_moon);
    if (curr_moon_sec == -1 && errno) {
        panic("couldn't mktime");
    }
    while (curr_moon.tm_wday != MONDAY) {
        ++curr_moon.tm_mday;
        curr_moon.tm_isdst = -1;
        errno = 0;
        curr_moon_sec = mktime(&curr_moon);
        if (curr_moon_sec == -1 && errno) {
            panic("couldn't mktime");
        }
    }
    curr_moon.tm_mday += CORRECT_DAY_OFFSET;
    curr_moon.tm_isdst = -1;
    errno = 0;
    curr_moon_sec = mktime(&curr_moon);
    if (curr_moon_sec == -1 && errno) {
        panic("couldn't mktime");
    }
    char buf[OUT_BUF_SIZE];
    if (strftime(buf, sizeof(buf), "%F", &curr_moon) == 0) {
        panic("couldn't strftime");
    }
    puts(buf);
}
