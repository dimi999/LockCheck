.DEFAULT_GOAL := exec

CC = gcc
CFLAGS = -Wall

DEPS = parser.h banker.h
OBJ = parser.o banker.o test.o

$.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

exec: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

clean:
	rm $(OBJ) exec