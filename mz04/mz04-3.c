#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

enum CONSTANTS
{
    ARGC = 2
};

int
main(int argc, char **argv)
{
    if (argc != ARGC) {
        fprintf(stderr, "Error: incorrect argument amount\n");
        exit(1);
    }
    int in = open(argv[1], O_RDWR, 0);
    if (in == -1) {
        fprintf(stderr, "Error: couldn't open file\n");
        exit(1);
    }
    long long curr_num = 0;
    ssize_t cnt = 0;
    off_t pos = 0;
    off_t curr_pos = -1;
    long long curr_ans = 0;
    while ((cnt = read(in, &curr_num, sizeof(long long)))) {
        if (cnt == -1) {
            fprintf(stderr, "Error: couldn't read from file\n");
            exit(1);
        }
        if (cnt != sizeof(long long)) {
            fprintf(stderr, "Error: wrong input file format\n");
            exit(1);
        }
        if (curr_pos == -1 || curr_num < curr_ans) {
            curr_ans = curr_num;
            curr_pos = pos;
        }
        pos += sizeof(long long);
    }
    if (curr_pos == -1) {
        return 0;
    }
    if (lseek(in, curr_pos, SEEK_SET) == -1) {
        fprintf(stderr, "Error: couldn't set position in file\n");
        exit(1);
    }
    if (curr_ans != LLONG_MIN) {
        curr_ans = -curr_ans;
    }

    if (write(in, &curr_ans, sizeof(long long)) == -1) {
        fprintf(stderr, "Error: couldn't write to file\n");
        exit(1);
    }

    if (close(in) == -1) {
        fprintf(stderr, "Error: couldn't close file\n");
        exit(1);
    }
}
