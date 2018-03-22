CC=g++
SRC=$(shell find . -name '*.cc')
OBJ=$(SRC:.cc=.o)
DEP=$(shell find . -name '*.h')
CFLAGS=-Wall -ggdb -Wl,-subsystem,windows -O0
LINKFLAGS=
LIBS=-lmingw32
TARGET=test.exe

.PHONY: all clean

%.o: %.cc $(DEP)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJ)
	$(MAKE) -C ../src all
	$(eval MAIN:=$(shell find ../src -name '*.cc'))
	$(CC) $(LINKFLAGS) $(OBJ) $(MAIN:.cc=.o) -o $(TARGET) $(LIBS)

clean:
	rm -f $(OBJ) $(TARGET)

