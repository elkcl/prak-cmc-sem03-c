#include <stdio.h>
#include <unistd.h>
int main(void) {
    int fd[2];
    pipe(fd);
    FILE *in = fdopen(fd[0], "r");
    FILE *out = fdopen(fd[1], "w");
    setbuf(in, NULL);
    fprintf(out, "%daboba\n", 5);
    fclose(out);
    int x;
    fscanf(in, "%d", &x);
    printf("%d\n", x);
    char buf[10000];
    fscanf(in, "%s", buf);
    fclose(in);
    puts(buf);
}
