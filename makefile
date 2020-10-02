СС=gcc
CFLAGS=-c -Wall -Werror
%: %.c
    $(CC) $@.c -o $@ $(CFLAGS) -fsanitize=address,leak
