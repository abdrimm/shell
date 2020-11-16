all:
	gcc sources/main.c -o sources/main -Wall -Werror

clean:
	rm sources/main
