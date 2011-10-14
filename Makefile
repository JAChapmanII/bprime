LDFLAGS=
CFLAGS=-pedantic -ansi -Wall -Wextra

bprime: bprime.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o bprime

