 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <err.h>

char *get_word(char *end) {
    char *word = NULL;
    char ch = getchar();
    int i = 0;
    if (*end == '\n') {
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
    char **arr = NULL;
    char end;
    char *word = get_word(&end);
    int i = 0;
    while (word) {
        if (end == '\n') {
            if (strlen(word) == 0) {
                arr = realloc(arr, (i + 1) * sizeof(char*));
                arr[i] = NULL;
                free(word);
            } else {
                arr = realloc(arr, (i + 2) * sizeof(char*));
                arr[i] = word;
                arr[i + 1] = NULL;
            }
            break;
        }
        i++;
        arr = realloc(arr, i * sizeof(char*));
        arr[i - 1] = word;
        word = get_word(&end);
    }
    arr = realloc(arr, (i + 1) * sizeof(char*));
    arr[i - 1] = NULL;
    return arr;
}

void redirection(char **list) {
    int i = 0, fd_in = 0, fd_out = 1;
    while (list[i] != NULL) {
        if (strcmp(list[i], "<") == 0) {
            fd_in = open(list[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("Open failed");
                exit(1);
            }
            break;
        } else if (strcmp(list[i], ">") == 0) {
            fd_out = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
                                            S_IRUSR | S_IWUSR);
            if (fd_out < 0) {
                perror("Open failed");
                exit(1);
            }
            break;
        }
        i++;
    }
    list = realloc(list, i * sizeof(char*));
    list[i] = NULL;
    return;
}

void clear(char **list) {
    int i = 0;
    while (list[i] != NULL) {
        free(list[i]);
        i++;
    }
    free(list);
}

int main() {
     char **list;
     while(1) {
         list = get_list();
         if ((strcmp(list[0], "exit") == 0) || (strcmp(list[0], "quit") == 0)) {
             clear(list);
             break;
         }
         redirection(list);
         clear(list);
     }
     clear(list);
     return 0;
}
