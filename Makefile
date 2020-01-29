CFLAGS := -Wall -Wpedantic -Werror --std=c11
LDFLAGS := -lncurses -lm

OBJS = snake.o

snake: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: all
all: snake

.PHONY: clean
clean:
	rm snake *.o
