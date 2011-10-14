LDFLAGS=-pthread -lm
CFLAGS=-pedantic -ansi -Wall -Wextra -g

bprime: bprime.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o bprime

