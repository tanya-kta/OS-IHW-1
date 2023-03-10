#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
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
    char used1[53];
    umask(0);
    mknod("FIFO-2", S_IFIFO|0666, 0);
    fp = fopen("FIFO-2", "r");
    fgets(used1, 53, fp);

    int file = open(argv[1], O_WRONLY);
    char str1[26];
    int n_bytes = processUsed(used1 + 26, str1);
    write(file, str1, n_bytes);
    write(file, "\n", 1);
    n_bytes = processUsed(used1, str1);
    write(file, str1, n_bytes);
}
