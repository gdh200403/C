CC=g++
CFLAGS=-I. -g -std=c++17
VPATH=src
OBJ=assembler.o main.o
DEPS = assembler.h

assembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: assembler

.PHONY: clean

clean:
	rm -rf assembler
	rm *.o
