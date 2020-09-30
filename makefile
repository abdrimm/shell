СС=gcc
CFLAGS=-c -Wall -Werror
SOURCES:
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=
all: $(PROG)
%: %.c
    $(CC) $@.c -o $@ $(CFLAGS) -fsanitize=address,leak
 clean:
     rm $(PROG)
