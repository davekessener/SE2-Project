CC=g++
SRC=$(shell find . -name '*.cc')
OBJ=$(SRC:.cc=.o)
DEP=$(shell find . -name '*.h')
CFLAGS=-Wall -ggdb -Wl,-subsystem,windows -O0

.PHONY: all clean

%.o: %.cc $(DEP)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJ)

clean:
	rm -f *.o

