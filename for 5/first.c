#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv) {
    FILE *fp;
    char str[5010];
    umask(0);
    mknod("FIFO-1", S_IFIFO|0666, 0);
    int file = open(argv[1], O_RDONLY);
    int n_bytes = read(file, str, 5000);
    str[n_bytes] = '\0';

    fp = fopen("FIFO-1", "w");
    fputs(str, fp);
    fclose(fp);
    return 0;
}