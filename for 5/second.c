#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void processString(char *str, int n_bytes, char *used1, char *used2) {
    int id = 0;
    for (int i = 0; i < 26; ++i) {
        used1[i] = '0';
        used2[i] = '0';
    }
    while (id < n_bytes && str[id] != '\n') {
        used1[str[id] - 'a'] = '1';
        ++id;
    }
    ++id;
    while (id < n_bytes && str[id] != '\0') {
        used2[str[id] - 'a'] = '1';
        ++id;
    }
}

int main(int argc, char **argv) {
    FILE *fp;
    char str[5010], str2[5010];
    umask(0);
    mknod("FIFO-1", S_IFIFO|0666, 0);
    fp = fopen("FIFO-1", "r");
    fgets(str, 5010, fp);
    fgets(str2, 5010, fp);
    strcat(str, str2);
    //printf("%s\n", str);
    fclose(fp);

    char used1[53], used2[27];
    int n_bytes = strlen(str);
    processString(str, n_bytes, used1, used2);
    used1[26] = '\0';
    used2[26] = '\0';
    strcat(used1, used2);
    used1[52] = '\0';
    umask(0);
    mknod("FIFO-2", S_IFIFO|0666, 0);
    fp = fopen("FIFO-2", "w");
    fputs(used1, fp);
    fclose(fp);
    return 0;
}
