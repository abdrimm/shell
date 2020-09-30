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
    while (ch != ' ' && ch != '\t' && ch != '\n') {
        i++;
        word = realloc(word, i * sizeof(char));
        word[i - 1] = ch;
    }
    word = realloc(word, (i + 1) * sizeof(char));
    word[i] = '\0';
    *end = ch;
    return word;
}

char **get_list() {
    char **list = NULL;
    char end;
    char *ch = get_word(&end);
    int i = 0;
    while (end != '\n') {
        i++;
        list = realloc(list, i);
        list[i - 1] = ch;
        ch = get_word(&end);
    }
    list = realloc(list, i + 1);
    list[i] = NULL;
    return list;
}

void redirection(char **list) {
    int i = 0, fd_in = 0, fd_out = 1;
    while (list[i] != NULL) {
        if (strcmp(list[i], "<") == 0) {
            fd_in = open(list[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("'<'(open) failed");
                exit(1);
            }
            break;
        }
        else if (strcmp(list[i], ">") == 0) {
            fd_out = open(list[i + 1], O_WRONLY | O_CREAT | O_TRUNC,
                                            S_IRUSR | S_IWUSR);
            if (fd_out < 0) {
                perror("'>'(open) failed");
                exit(1);
            }
            break;
        }
        i++;
    }
    list = realloc(list, (i + 1) * sizeof(char*));
    list[i] = NULL;
    return;
}
