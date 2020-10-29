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
    if (ch == '\n') {
        return NULL;
    }
    while (ch != ' ' && ch != '\t' && ch != '\n') {
        word = realloc(word, (i + 1) * sizeof(char));
        if (!word) {
            printf("word error");
            exit(1);
        }
        word[i] = ch;
	    i++;
        ch = getchar();
    }
    word = realloc(word, (i + 1) * sizeof(char));
    word[i] = '\0';
    *end = ch;
    return word;
}

char **get_list() {
    char end;
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

void redirection(char **list, int x) {
    int i = x, fd_in = 0, fd_out = 1;
    while (list[i]) {
        if (strcmp(list[i], "<") == 0) {
            fd_in = open(list[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("Open failed");
                exit(1);
            }
            dup2(fd_in, 0);
            close(fd_in);
            free(list[i]);
            free(list[i + 1]);
            list[i] = NULL;
            list[i + 1] = NULL;
            break;
        } else if (strcmp(list[i], ">") == 0) {
            fd_out = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
                                            S_IRUSR | S_IWUSR);
            if (fd_out < 0) {
                perror("Open failed");
                exit(1);
            }
            dup2(fd_out, 1);
            close(fd_out);
            free(list[i]);
            free(list[i + 1]);
            list[i] = NULL;
            list[i + 1] = NULL;
            break;
        }
        i++;
    }
    return;
}

int is_exit(char **list) {
    if (list && ((strcmp(list[0], "exit") && strcmp(list[0], "quit")) || (!list[0]))) {
        return 1;
    } else {
        return 0;
    }
}

int change_dir(char **list) {
    char *home = getenv("HOME");
    if (strcmp(list[0], "cd") == 0) {
        if (list[1] == NULL || (strcmp(list[1], "~") == 0)) {
            chdir(home);
        } else {
            chdir(list[1]);
        }
        return 1;
    }
    return 0;
}

void clear(char **list) {
    int i = 0;
    while (list[i] != NULL) {
        free(list[i]);
        i++;
    }
    free(list);
}

int pipe_lines(char **list, int **x) {
    int i = 0;
    int str_num = 0;
    while (list[i]) {
        if (strcmp(list[i], "|") == 0) {
            free(list[i]);
            list[i] = NULL;
            str_num++;
            *x = realloc(*x, (str_num + 1) * sizeof(int));
            (*x)[str_num] = i + 1;
        }
        i++;
    }
    return str_num;
}

void pipes(char **list) {
    int *x = malloc(sizeof(int));
    x[0] = 0;
    int i;
    int str_num = pipe_lines(list, &x);
    if (str_num <= 1) {
        return;
    }
    int (*fd)[2] = malloc((str_num + 1) * sizeof(int[2]));
    for (i = 0; i <= str_num; i++) {
        pipe(fd[i]);
	if (change_dir(list) == 1)
		continue;
    }
    if (fork() == 0) {
        if (str_num) {
            dup2(fd[0][1], 1);
        }
        close(fd[0][1]);
        close(fd[0][0]);
        redirection(list, x[0]);
        if (execvp(list[0], list) < 0) {
            perror("exec failed");
            return;
        }
    }
    for (i = 1; i <= str_num; i++) {
        if (fork() == 0) {
            dup2(fd[i - 1][0], 0);
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
            if (i != str_num) {
                dup2(fd[i][1], 1);
            }
            close(fd[i][1]);
            close(fd[i][0]);
            redirection(list, x[i]);
            if (execvp(list[0], list) < 0) {
                perror("exec failed");
                return;
            }
        } else {
            close(fd[i - 1][1]);
            close(fd[i - 1][0]);
        }
    }
    close(fd[str_num][1]);
    close(fd[str_num][0]);
    for (i = 0; i <= str_num; i++) {
        wait(NULL);
    }
    free(fd);
    clear(list);
    free(x);
  //  x = malloc(sizeof(int));
  //  x[0] = 0;
    list = get_list();
}

int main() {
     char **list = get_list();
     while (is_exit(list)) {
            if (fork() > 0) {
                wait(NULL);
            } else {
                pipes(list);
                /*redirection(list);
                if (execvp(list[0], list) < 0) {
                    perror("exec failed");
                    return 1;
                }*/
            }
            clear(list);
            list = get_list();
     }
     clear(list);
     return 0;
}
