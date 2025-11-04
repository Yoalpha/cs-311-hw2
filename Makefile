CC = gcc-15
CFLAGS = -g -O2 -pthread
MAIN ?= main.c

all: thread_sync
thread_sync: main.o user_data.o
	$(CC) $(CFLAGS) -o thread_sync main.o user_data.o

main.o: $(MAIN) user_data.h
	$(CC) $(CFLAGS) -c $(MAIN) -o main.o

user_data.o: user_data.c user_data.h
	$(CC) $(CFLAGS) -c user_data.c -o user_data.o

clean:
	rm -f *.o thread_sync
