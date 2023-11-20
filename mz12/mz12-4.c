#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

const char CODE1[] = "#!/usr/bin/python3\n"
                     "import os\n"
                     "print(";
const char CODE2[] = ")\n"
                     "os.remove(";
const char CODE3[] = ")\n";

void
panic(char *err)
{
    if (errno) {
        dprintf(STDERR_FILENO, "Error: %s\nLast errno: %s\n", err, strerror(errno));
    } else {
        dprintf(STDERR_FILENO, "Error: %s\n", err);
    }
    _exit(1);
}

ssize_t
read_all(int fd, void *buf_v, size_t size)
{
    char *buf = buf_v;
    ssize_t remaining = size;
    while (remaining > 0) {
        ssize_t cnt_read = 0;
        do {
            errno = 0;
            cnt_read = read(fd, buf, remaining);
        } while (cnt_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
        if (cnt_read == -1) {
            panic("reading from file failed");
        }
        if (cnt_read == 0) {
            break;
        }
        remaining -= cnt_read;
        buf += cnt_read;
    }
    return size - remaining;
}

void
write_all(int fd, const void *buf_v, size_t size)
{
    const char *buf = buf_v;
    while (size > 0) {
        ssize_t written = 0;
        do {
            errno = 0;
            written = write(fd, buf, size);
        } while (written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
        if (written == -1) {
            panic("writing to file failed");
        }
        size -= written;
        buf += written;
    }
}

int
main(int argc, char *argv[])
{
    char *tmpdir = getenv("XDG_RUNTIME_DIR");
    if (tmpdir == NULL) {
        tmpdir = getenv("TMPDIR");
        if (tmpdir == NULL) {
            tmpdir = "/tmp";
        }
    }
    int rd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (rd == -1) {
        panic("open failed");
    }
    char buf[] = "01234567890123456789.py";
    read_all(rd, buf, 20);
    for (int i = 0; i < 20; ++i) {
        buf[i] = buf[i] % 10 + '0';
    }
    char path[PATH_MAX];
    size_t path_len = 0;
    if ((path_len = snprintf(path, PATH_MAX, "%s/%s", tmpdir, buf)) >= PATH_MAX) {
        panic("path too long");
    }
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0770);
    write_all(fd, CODE1, sizeof(CODE1) - 1);
    for (int i = 1; i < argc; ++i) {
        write_all(fd, argv[i], strlen(argv[i]));
        if (i != argc - 1) {
            write_all(fd, "*", 1);
        }
    }
    write_all(fd, CODE2, sizeof(CODE2) - 1);
    write_all(fd, path, path_len);
    write_all(fd, CODE3, sizeof(CODE3) - 1);
    if (close(fd) == -1) {
        panic("close failed");
    }
    execlp(path, path, NULL);
    panic("exec failed");
}
