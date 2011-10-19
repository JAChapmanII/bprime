LDFLAGS=-pthread -lm
CFLAGS=-pedantic -ansi -Wall -Wextra

ifdef RELEASE
CFLAGS+=-O3 -Os
else
CFLAGS=-g
endif

bprime: bprime.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o bprime

