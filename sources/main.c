#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

char *get_word(char *end) {
    char *word = NULL;
    char ch = getchar();
    int i = 0;
    if (*end == '\n') {
        return NULL;
    }
    while (ch != ' ' && ch != '\n') {
        i++;
        word = realloc(word, (i + 1) * sizeof(char));
        if (!word) {
            printf("word error");
            exit(1);
        }
        word[i - 1] = ch;
        ch = getchar();
    }
    word[i] = '\0';
    *end = ch;
    return word;
}

char **get_list() {
    char end = 0;
    char **list = NULL;
    char *word = get_word(&end);
    int i = 0;
    while (1) {
        if (end == '\n') {
            if (strlen(word) == 0) {
                list  = realloc(list, (i + 1) * sizeof(char*));
                list[i] = NULL;
                free(word);
            } else {
                list = realloc(list, (i + 2) * sizeof(char*));
                list[i] = word;
                list[i + 1] = NULL;
            }
            break;
        }
        i++;
        list = realloc(list, i * sizeof(char*));
        list[i - 1] = word;
        word = get_word(&end);
    }
    return list;
}

int redirection(char **list, int x) {
    int i = x;
    char fname[100];
    while (list[i]) {
        if (strcmp(list[i], ">") == 0) {
            strcpy(fname, list[i+1]);
            int fd1 = open(fname,
                O_WRONLY | O_CREAT | O_TRUNC ,
                S_IRUSR | S_IWUSR);
            dup2(fd1, 1);
            free(list[i]);
            free(list[i+1]);
            list[i] = NULL;
            list[i + 1] = NULL; 
            i++;
            close(fd1);
        }
        if (list[i] && strcmp(list[i], "<") == 0) {
            strcpy(fname, list[i+1]);
            int fd2 =  open(fname, O_RDONLY);
            dup2(fd2, 0);
            free(list[i]);
            free(list[i+1]);
            list[i] = NULL;
            list[i + 1] = NULL;
            i++;
            close(fd2);
        }
        i++;
    }
    return 0;
}

int pipe_line(char **list, int **x) {
    int i = 0;
    int cnt = 0;
    while (list[i]) {
        if (strcmp(list[i], "|") == 0) {
            free(list[i]);
            list[i] = NULL;
            cnt++;
            *x = realloc(*x, (cnt + 1) * sizeof(int));
            (*x)[cnt] = i + 1;
        }
        i++;
    }
    return cnt;
}

int is_exit(char **list) {
    if (!list[0] || (strcmp(list[0], "exit") && strcmp(list[0], "quit"))) {
        return 1;
    } else {
        return 0;
    }
}

int execute(char **list, int x) {
    if (execvp(list[x], list + x) < 0) {
        perror("exec failed");
        return 1;
    }
    return 0;
}


void clear(char **list, int *x, int cnt) {
    for (int j = 0; j < cnt + 1; j++) {
        int i = x[j];
        while (list[i]) {
            free(list[i]);
            i++;
        }
    }
    free(list);
}

int main() {
    int *x;
    x = malloc(1 * sizeof(int));
    x[0] = 0;
    int cnt;
    char **list = get_list();
    while (!list[0] || (strcmp(list[0], "exit") && strcmp(list[0], "quit"))) {
        if (list[0]) {
            cnt = pipe_line(list, &x);
            int (*fd)[2] = malloc((cnt + 1) * sizeof(int[2]));
            for (int i = 0; i <= cnt; i++) {
                pipe(fd[i]);
            }
            for (int i = 0; i <= cnt; i++) {
                if (fork() == 0) {
                    if (i)
                        dup2(fd[i - 1][0], 0);
                    if (cnt && i != cnt)
                        dup2(fd[i][1], 1);
                    for (int j = cnt; j >= 0; j--) {
                        close(fd[j][1]);
                        close(fd[j][0]);
                    }
                    redirection(list, x[i]); 
                    return execute(list, x[i]);
                } else if (i) {
                    close(fd[i - 1][1]);
                    close(fd[i - 1][0]);
                }
            }
            close(fd[cnt][1]);
            close(fd[cnt][0]);
            for (int i = 0; i <= cnt; i++) {
                wait(NULL);
            }
            free(fd);
        }
        clear(list, x, cnt);
        free(x);
        x = malloc(1 * sizeof(int));
        x[0] = 0;
        list = get_list();
    }
    clear(list, x, 0);
    free(x);
    return 0;
}
