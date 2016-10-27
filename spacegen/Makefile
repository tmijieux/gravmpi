TARGET=spacegen
CFLAGS=-std=gnu99 -g -Wall -Wextra
LDFLAGS=
GENGETOPT=gengetopt

SRC= \
	spacegen.c \
	cmdline.c

OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)

all: $(TARGET)

-include $(DEP)


cmdline.h cmdline.c: spacegen.ggo
	$(GENGETOPT) < $^

spacegen: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
	$(CC) -c $(CFLAGS) $*.c -o $*.o

clean:
	$(RM) $(TARGET) $(OBJ) $(DEP) *.d *.o

mrproper: clean
	$(RM) $(TARGET)

