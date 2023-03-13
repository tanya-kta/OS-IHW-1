#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int processUsed(char *used, char *str1) {
    int id = 0;
    for (int i = 0; i < 26; ++i) {
        if (used[i] == '0') {
            str1[id] = 'a' + i;
            ++id;
        }
    }
    return id;
}

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



    char used1[53];
    umask(0);
    mknod("FIFO-2", S_IFIFO|0666, 0);
    fp = NULL;
    while (fp == NULL) {
        sleep(1);
        fp = fopen("FIFO-2", "r");
    }

    fgets(used1, 53, fp);
    fclose(fp);

    file = open(argv[2], O_WRONLY | O_CREAT);
    char str1[26];
    n_bytes = processUsed(used1 + 26, str1);
    write(file, str1, n_bytes);
    write(file, "\n", 1);
    n_bytes = processUsed(used1, str1);
    write(file, str1, n_bytes);

    return 0;
}