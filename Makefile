CC=gcc
CC_OPT=-std=c11

CC_OPTS=-Wall -ansi
CC_MATH=-lm

DEPS = utils.h K128.h
OBJ = utils.o K128.o

.PHONY: all
all: utils.o K128.o  main

utils.o: utils.c
	$(CC) -c utils.c

K128.o: K128.c
	$(CC) -c K128.c

main: $(DEPS) $(OBJ) main.c
	$(CC) $(CC_OPTS) $(CC_OPT) -o main $(OBJ) main.c $(CC_MATH) -no-pie

run:
	main

clean:
	rm -f main *.o