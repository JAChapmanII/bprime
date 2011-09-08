LDFLAGS=
CFLAGS=-pedantic -ansi -Wall -Wextra

seratosthenes: seratosthenes.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o seratosthenes

