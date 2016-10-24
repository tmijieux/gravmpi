TARGET=star
CFLAGS=-std=gnu99 -g -Wall -Wextra $(shell pkg-config --cflags glib-2.0)
LDFLAGS=$(shell pkg-config --libs glib-2.0) -lm
GENGETOPT=gengetopt
CC=mpicc

ifdef DEBUG

else
CFLAGS+=-O3
endif

SRC=    cmdline.c \
	main.c \
	local.c \
	mpi.c \
	input.c \
	star.c

OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: $(TARGET)

-include $(DEP)


star: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
	$(CC) -c $(CFLAGS) $*.c -o $*.o

clean:
	$(RM) $(TARGET) $(OBJ) $(DEP) *.d *.o

mrproper: clean
	$(RM) $(TARGET)

