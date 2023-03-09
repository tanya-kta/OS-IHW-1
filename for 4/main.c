#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        if (used[i] == 1) {
            str[id] = 'a' + i;
            ++id;
        }
    }
    return id;
}

int main(int argc, char **argv) {
    int fd1[2];
    pid_t chpid1;

    pipe(fd1);
    chpid1 = fork();
    if (chpid1 == -1) {
        printf("Creating the first child went wrong\n");
        exit(-1);
    } else if (chpid1 == 0) {
        int fd2[2];
        pid_t chpid2;
        pipe(fd2);
        chpid2 = fork();
        if (chpid2 == -1) {
            printf("Creating the second child went wrong\n");
            exit(-1);
        } else if (chpid2 == 0) {
            int file = open(argv[1], O_RDONLY);
            char str[5010];
            int n_bytes = read(file, str, 5000);
            str[n_bytes] = '\0';
            printf("%s\n", str);

            close(fd2[0]);
            write(fd2[1], str, n_bytes + 1);
            exit(0);
        } else {
            close(fd2[1]);
            char str[5010];
            int n_bytes = read(fd2[0], str, 5000);
            char used2[26];
            char used1[26];
            processString(str, n_bytes, used1, used2);
            close(fd1[0]);
            write(fd1[1], used1, sizeof(used1));
            write(fd1[1], used2, sizeof(used2));
            exit(0);
        }
    } else {
        close(fd1[1]);
        char used1[26];
        char used2[26];
        read(fd1[0], used1, sizeof(used1));
        read(fd1[0], used2, sizeof(used2));
        int file = open(argv[2], O_WRONLY);
        char str[26];
        int n_bytes = processUsed(used2, str);
        write(file, str, n_bytes);
        write(file, "\n", 1);
        n_bytes = processUsed(used1, str);
        write(file, str, n_bytes);
    }

    return 0;
}