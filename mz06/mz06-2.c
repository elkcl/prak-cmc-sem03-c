#include <stddef.h>  // size_t
#include <stdio.h>   // fprintf
#include <string.h>  // strerror
#include <stdlib.h>  // exit()
#include <errno.h>   // errno
#include <stdbool.h> // bool

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

bool
check_curr_dir(const char *str)
{
    if (str[0] != '.') {
        return false;
    }
    if (str[1] != '/' && str[1] != '\0') {
        return false;
    }
    return true;
}

bool
check_prev_dir(const char *str)
{
    if (str[0] != '.') {
        return false;
    }
    if (str[1] != '.') {
        return false;
    }
    if (str[2] != '/' && str[2] != '\0') {
        return false;
    }
    return true;
}

void
normalize_path(char *buf)
{
    if (buf[0] != '/') {
        panic("incorrect input format");
    }
    char *last = buf + 1;
    const char *curr = buf;
    while (curr[0] != '\0' && curr[1] != '\0') {
        ++curr;
        if (check_curr_dir(curr)) {
            ++curr;
            continue;
        }
        if (check_prev_dir(curr)) {
            if (last - 1 != buf) {
                last -= 2;
                while (*last != '/') {
                    --last;
                }
                ++last;
            }
            curr += 2;
            continue;
        }
        while (*curr != '\0' && *curr != '/') {
            char tmp = *curr;
            *last = tmp;
            ++curr;
            ++last;
        }
        if (*curr == '/') {
            *last = '/';
            ++last;
        } else {
            *last = '\0';
            break;
        }
    }
    if (*last == '\0') {
        if (last - 1 != buf && *(last - 1) == '/') {
            --last;
            *last = '\0';
        }
    } else {
        if (last - 1 == buf) {
            *last = '\0';
        } else {
            --last;
            *last = '\0';
        }
    }
}
