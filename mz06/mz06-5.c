#include <linux/limits.h> // PATH_MAX
#include <string.h>       // strlen()
#include <sys/types.h>    // ino_t
#include <sys/stat.h>     // stat()
#include <dirent.h>       // readdir()
#include <unistd.h>       // fchdir()
#include <stdbool.h>      // bool
#include <stdio.h>        // snprintf

const char PREV_DIR[] = "../";

enum
{
    CURR_LEN = 2,
    PREV_SIZE = sizeof(PREV_DIR)
};

ssize_t
getcwd2(int fd, char *buf, size_t size)
{
    DIR *init_cwd = opendir(".");
    if (init_cwd == NULL) {
        return -1;
    }
    int fd_cwd = dirfd(init_cwd);
    if (fd_cwd == -1) {
        closedir(init_cwd);
        return -1;
    }
    if (fchdir(fd) == -1) {
        closedir(init_cwd);
        return -1;
    }

    char prev_path[PATH_MAX] = "./";
    char cat_buf[PATH_MAX];
    size_t prev_eptr = 2;
    struct stat info = {};
    if (lstat(prev_path, &info) == -1) {
        fchdir(fd_cwd);
        closedir(init_cwd);
        return -1;
    }
    ino_t prev_ino = info.st_ino;
    ino_t curr_ino = info.st_ino;
    dev_t prev_dev = info.st_dev;
    dev_t curr_dev = info.st_dev;
    do {
        prev_ino = curr_ino;
        prev_dev = curr_dev;
        if (prev_eptr + PREV_SIZE - 1 >= PATH_MAX) {
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
        memcpy(prev_path + prev_eptr, PREV_DIR, PREV_SIZE);
        prev_eptr += PREV_SIZE - 1;
        if (lstat(prev_path, &info) == -1) {
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
        curr_ino = info.st_ino;
        curr_dev = info.st_dev;
    } while (curr_ino != prev_ino || curr_dev != prev_dev);
    prev_eptr -= PREV_SIZE - 1;
    prev_path[prev_eptr] = '\0';

    size_t buf_eptr = 0;
    size_t remaining = size;
    ssize_t ans = 0;
    if (prev_eptr == 2) {
        if (remaining == 1) {
            buf[0] = '\0';
        } else if (remaining > 1) {
            buf[0] = '/';
            buf[1] = '\0';
        }
        if (fchdir(fd_cwd) == -1) {
            closedir(init_cwd);
            return -1;
        }
        if (closedir(init_cwd) == -1) {
            return -1;
        }
        return 1;
    }
    while (prev_eptr != 2) {
        DIR *curr_dir = opendir(prev_path);
        if (curr_dir == NULL) {
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
        prev_eptr -= PREV_SIZE - 1;
        prev_path[prev_eptr] = '\0';
        if (lstat(prev_path, &info) == -1) {
            closedir(curr_dir);
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
        curr_ino = info.st_ino;
        curr_dev = info.st_dev;
        struct dirent *curr_dirent;
        do {
            curr_dirent = readdir(curr_dir);
            if (curr_dirent == NULL) {
                break;
            }
            if (snprintf(cat_buf, PATH_MAX, "%s/../%s", prev_path, curr_dirent->d_name) >= PATH_MAX) {
                closedir(curr_dir);
                fchdir(fd_cwd);
                closedir(init_cwd);
                return -1;
            }
            if (lstat(cat_buf, &info) == -1) {
                closedir(curr_dir);
                fchdir(fd_cwd);
                closedir(init_cwd);
                return -1;
            }
            if (!S_ISDIR(info.st_mode)) {
                continue;
            }
        } while (curr_ino != info.st_ino || curr_dev != info.st_dev);
        if (curr_dirent == NULL) {
            closedir(curr_dir);
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
        size_t curr_len = strlen(curr_dirent->d_name);
        ans += 1 + curr_len;
        if (remaining > 1) {
            buf[buf_eptr] = '/';
            ++buf_eptr;
            --remaining;
        }
        if (remaining > 1) {
            if (curr_len <= remaining - 1) {
                memcpy(buf + buf_eptr, curr_dirent->d_name, curr_len);
                buf_eptr += curr_len;
                remaining -= curr_len;
            } else {
                memcpy(buf + buf_eptr, curr_dirent->d_name, remaining - 1);
                buf_eptr += remaining - 1;
                remaining = 1;
            }
        }
        if (closedir(curr_dir) == -1) {
            fchdir(fd_cwd);
            closedir(init_cwd);
            return -1;
        }
    }
    if (remaining > 0) {
        buf[buf_eptr] = '\0';
    }

    if (fchdir(fd_cwd) == -1) {
        closedir(init_cwd);
        return -1;
    }
    if (closedir(init_cwd) == -1) {
        return -1;
    }
    return ans;
}

int
main(int argc, char **argv)
{
    DIR *init_dir = opendir("/dev");
    char buf[PATH_MAX];
    ssize_t ans = getcwd2(dirfd(init_dir), buf, PATH_MAX);
    printf("%ld\n", ans);
    if (ans != -1) {
        puts(buf);
    }
    closedir(init_dir);
}
