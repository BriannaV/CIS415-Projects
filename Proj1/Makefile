all: pseudo-shell

pseudo-shell: main.o command.o
	gcc -o pseudo-shell main.o command.o

main.o: main.c command.h
	gcc -c main.c

command.o: command.c command.h
	gcc -c command.c

clean:
	rm -f core *.o pseudo-shell
