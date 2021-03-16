CC = g++
CFLAGS = -c -Wall -std=c++17
TEST = -pg
LIBS = -lc
LFLAGS = -c

all: LinearTable.o main.o
	$(CC) $^ -o  $@ $(LIBS)

LinearTable.o: LinearTable.cpp
	$(CC) $(CFLAGS) $<
main.o: main.cpp
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o