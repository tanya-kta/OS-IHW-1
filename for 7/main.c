#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
    pid_t chpid1;

    chpid1 = fork();
    if (chpid1 == -1) {
        printf("Creating the first child went wrong\n");
        exit(-1);
    } else if (chpid1 == 0) {
        FILE *fp;
        char str[5010], str2[5010];
        umask(0);
        mknod("FIFO-1", S_IFIFO|0666, 0);
        fp = fopen("FIFO-1", "r");
        fgets(str, 5010, fp);
        fgets(str2, 5010, fp);
        strcat(str, str2);
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
        exit(0);
    } else {
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

        int status = 0;
        chpid1 = wait(&status);
        char used1[53];
        umask(0);
        mknod("FIFO-2", S_IFIFO|0666, 0);
        fp = fopen("FIFO-2", "r");
        fgets(used1, 53, fp);
        fclose(fp);

        file = open(argv[2], O_WRONLY | O_CREAT);
        char str1[26];
        n_bytes = processUsed(used1 + 26, str1);
        write(file, str1, n_bytes);
        write(file, "\n", 1);
        n_bytes = processUsed(used1, str1);
        write(file, str1, n_bytes);
    }

    return 0;
}