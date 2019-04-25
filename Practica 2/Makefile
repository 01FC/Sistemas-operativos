CC=gcc
all: init getty shell
init: init.c
	$(CC) -o init init.c
getty: getty.c
	$(CC) -o getty getty.c
shell: shell.c
	$(CC) -o shell shell.c
clean:
	rm init getty shell
