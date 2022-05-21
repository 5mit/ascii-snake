CC=gcc

LIBS = -lncurses

snake.o: snake.c
	$(CC) -o snake snake.c $(LIBS)

