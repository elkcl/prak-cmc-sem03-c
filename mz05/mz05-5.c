#include <stddef.h> // size_t
#include <stdio.h>  // fprintf
#include <string.h> // strerror
#include <stdlib.h> // exit()
#include <errno.h>  // errno

const char curr[] = ".";
const char prev[] = "..";

enum
{
    LEN_CURR = sizeof(curr) / sizeof(curr[0]) - 1,
    LEN_PREV = sizeof(prev) / sizeof(prev[0]) - 1
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

typedef const char *cptr_t;

cptr_t *
split_path(const char *path)
{
    size_t len = strlen(path);
    size_t n = 0;
    if (len > 1) {
        for (size_t i = 0; i < len; ++i) {
            if (path[i] == '/') {
                ++n;
            }
        }
    }
    cptr_t *ans = calloc(n + 1, sizeof(ans[0]));
    if (ans == NULL) {
        panic("couldn't allocate memory");
    }
    ans[n] = NULL;
    if (n == 0) {
        return ans;
    }
    size_t j = 0;
    for (size_t i = 0; i < len; ++i) {
        if (path[i] == '/') {
            ans[j] = path + i + 1;
            ++j;
        }
    }
    size_t last = 0;
    for (size_t i = 0; i < n; ++i) {
        if (strcmp(ans[i], curr) == 0) {
            continue;
        } else if (strcmp(ans[i], prev) == 0) {
            if (last > 0) {
                --last;
            }
        } else {
            ans[last] = ans[i];
            ++last;
        }
    }
    ans[last] = NULL;
    return ans;
}

char *
relativize_path(const char *path1, const char *path2)
{
    if (path1 == NULL || path2 == NULL || path1[0] != '/' || path2[0] != '/') {
        return NULL;
    }
    cptr_t *split1 = split_path(path1);
    cptr_t *split2 = split_path(path2);
    size_t len1 = 0;
    size_t len2 = 0;
    size_t pref = 0;
    for (size_t i = 0; split1[i] != NULL; ++i) {
        ++len1;
    }
    for (size_t i = 0; split2[i] != NULL; ++i) {
        ++len2;
    }
    for (size_t i = 0; i < len1 && i < len2 && split1[i] == split2[i]; ++i) {
        ++pref;
    }
    size_t len_ans = 0;
    if (len1 - pref == 0 && len2 - pref == 0) {
        len_ans = LEN_CURR;
    }
    if (len1 - pref != 0) {
        len_ans += (LEN_PREV + 1) * (len1 - pref);
    }
    if (len2 - pref != 0) {
        for (size_t i = pref; i < len2; ++i) {
            len_ans += strlen(split2[i]) + 1;
        }
    }
    if (len1 - pref != 0 && len2 - pref != 0) {
        --len_ans;
    }
    char *ans = calloc(len_ans + 1, sizeof(ans[0]));
    ans[len_ans] = '\0';
    if (len1 - pref == 0 && len2 - pref == 0) {
        ans[0] = '.';
    }
    size_t j = 0;
    if (len1 - pref != 0) {
        for (size_t i = 0; i < len1 - pref - 1; ++i) {
            ans[j] = '.';
            ans[j + 1] = '.';
            ans[j + 2] = '/';
        }
        ans[j] = '.';
        ans[j + 1] = '.';
        if (len2 - pref == 0) {
            j += 2;
        } else {
            ans[j + 2] = '/';
            j += 3;
        }
    }
    if (len2 - pref != 0) {
        for (size_t i = pref; i < len2 - 1; ++i) {
            size_t curr_len = strlen(split2[i]);
            memcpy(ans + j, split2[i], curr_len);
            ans[j + curr_len] = '/';
            j += curr_len + 1;
        }
        size_t curr_len = strlen(split2[len2 - 1]);
        memcpy(ans + j, split2[len2 - 1], curr_len);
        j += curr_len;
    }
    free(split1);
    free(split2);
    return ans;
}

int
main(int argc, char **argv)
{
    char *ans = relativize_path(argv[1], argv[2]);
    puts(ans);
    free(ans);
}
