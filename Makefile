TARGET=star
CFLAGS=-std=gnu99 -g -Wall -Wextra $(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0) -lm
GENGETOPT=gengetopt
CC=mpicc

SRC=    cmdline.c \
	main.c \
	local.c \
	mpi.c \
	input.c

all: $(TARGET)

-include $(DEP)

OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

star: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
	$(CC) -c $(CFLAGS) $*.c -o $*.o

clean:
	$(RM) $(TARGET) $(OBJ) $(DEP) *.d *.o

mrproper: clean
	$(RM) $(TARGET)

