SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c = .o)
CC   = gcc
#INCLUDES = -I/home/noah/build_sqlite3/include
#LIBS = -L/home/noah/build_sqlite3/lib -lsqlite3
CCFLAGS = -g -Wall -O0

coro_sched : $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< $(CCFLAGS) 
clean:
	rm *.o