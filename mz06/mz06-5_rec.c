#include <linux/limits.h> // PATH_MAX
#include <string.h>       // strlen()
#include <sys/types.h>    // ino_t
#include <sys/stat.h>     // stat()
#include <dirent.h>       // readdir()
#include <unistd.h>       // fchdir()
#include <stdbool.h>      // bool
#include <stdio.h>

ssize_t
getcwd_rec(char *buf, size_t size)
{
    struct stat curr_info = {};
    struct stat prev_info = {};
    if (lstat(".", &curr_info) == -1) {
        return -1;
    }
    if (lstat("..", &prev_info) == -1) {
        return -1;
    }
    if (curr_info.st_ino == prev_info.st_ino && curr_info.st_dev == prev_info.st_dev) {
        if (size > 0) {
            buf[0] = '/';
        }
        return 1;
    }
    if (chdir("..") == -1) {
        return -1;
    }
    ssize_t rec_ans = getcwd_rec(buf, size);
    if (rec_ans == -1) {
        return -1;
    }
    if (rec_ans >= size) {
        size = 0;
    } else {
        size -= rec_ans;
    }
    buf += rec_ans;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        return -1;
    }
    struct dirent *curr_ent;
    struct stat ent_info = {};
    do {
        curr_ent = readdir(dir);
        if (curr_ent == NULL) {
            break;
        }
        if (lstat(curr_ent->d_name, &ent_info) == -1) {
            closedir(dir);
            return -1;
        }
    } while (!S_ISDIR(ent_info.st_mode) || ent_info.st_ino != curr_info.st_ino || ent_info.st_dev != curr_info.st_dev);
    if (curr_ent == NULL) {
        closedir(dir);
        return -1;
    }
    size_t curr_len = strlen(curr_ent->d_name);
    rec_ans += 1 + curr_len;
    if (curr_len <= size) {
        memcpy(buf, curr_ent->d_name, curr_len);
        buf += curr_len;
        size -= curr_len;
    } else if (size > 0) {
        memcpy(buf, curr_ent->d_name, size);
        buf += size;
        size = 0;
    }
    if (size > 0) {
        buf[0] = '/';
    }
    if (chdir(curr_ent->d_name) == -1) {
        closedir(dir);
        return -1;
    }
    if (closedir(dir) == -1) {
        return -1;
    }
    return rec_ans;
}

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
    ssize_t rec_ans;
    if (size == 0) {
        rec_ans = getcwd_rec(buf, 0);
    } else {
        rec_ans = getcwd_rec(buf, size - 1);
    }
    if (rec_ans == -1) {
        fchdir(fd_cwd);
        closedir(init_cwd);
        return -1;
    }
    if (rec_ans == 1) {
        if (size > 1) {
            buf[1] = '\0';
        } else if (size == 1) {
            buf[0] = '\0';
        }
    } else {
        if (rec_ans - 1 < size - 1) {
            buf[rec_ans - 1] = '\0';
        } else if (size > 0) {
            buf[size - 1] = '\0';
        }
        --rec_ans;
    }

    if (fchdir(fd_cwd) == -1) {
        closedir(init_cwd);
        return -1;
    }
    if (closedir(init_cwd) == -1) {
        return -1;
    }
    return rec_ans;
}

int
main(int argc, char **argv)
{
    DIR *init_dir = opendir(argv[1]);
    size_t sz;
    sscanf(argv[2], "%zu", &sz);
    char buf[PATH_MAX] = {0};
    ssize_t ans = getcwd2(dirfd(init_dir), buf, sz);
    printf("%ld\n", ans);
    if (ans != -1) {
        puts(buf);
    }
    closedir(init_dir);
}
