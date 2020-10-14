all:
	gcc sources/main.c -o sources/main -Wall -Werror -lm -fsanitize=address,leak

clean:
	rm sources/main
