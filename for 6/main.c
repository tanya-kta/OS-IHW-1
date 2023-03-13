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
        used1[i] = 0;
        used2[i] = 0;
    }
    while (id < n_bytes && str[id] != '\n') {
        used1[str[id] - 'a'] = 1;
        ++id;
    }
    ++id;
    while (id < n_bytes && str[id] != '\0') {
        used2[str[id] - 'a'] = 1;
        ++id;
    }
}

int processUsed(char *used, char *str) {
    int id = 0;
    for (int i = 0; i < 26; ++i) {
        if (used[i] == 0) {
            str[id] = 'a' + i;
            ++id;
        }
    }
    return id;
}

int main(int argc, char **argv) {
    int fd1[2], fd2[2];
    pid_t chpid1;

    pipe(fd1);
    pipe(fd2);
    chpid1 = fork();
    if (chpid1 == -1) {
        printf("Creating the first child went wrong\n");
        exit(-1);
    } else if (chpid1 == 0) {
        pid_t chpid2;
        close(fd2[1]);
        char str[5010];
        int n_bytes = read(fd2[0], str, 5000);
        char used2[26];
        char used1[26];
        printf("%s\n", str);
        processString(str, n_bytes, used1, used2);
        close(fd1[0]);
        write(fd1[1], used1, sizeof(used1));
        write(fd1[1], used2, sizeof(used2));
        exit(0);
    } else {
        int file = open(argv[1], O_RDONLY);
        char str[5010];
        int n_bytes = read(file, str, 5000);
        str[n_bytes] = '\0';
        close(fd2[0]);
        write(fd2[1], str, n_bytes + 1);
        int status = 0;
        chpid1 = wait(&status);

        close(fd1[1]);
        char used1[26];
        char used2[26];
        read(fd1[0], used1, sizeof(used1));
        read(fd1[0], used2, sizeof(used2));
        file = open(argv[2], O_WRONLY|O_CREAT);
        char str2[26];
        n_bytes = processUsed(used2, str2);
        write(file, str2, n_bytes);
        write(file, "\n", 1);
        n_bytes = processUsed(used1, str2);
        write(file, str2, n_bytes);
    }

    return 0;
}