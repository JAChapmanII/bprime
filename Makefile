LDFLAGS=-lm `sdl-config --libs` -lGL
CFLAGS=-pedantic -ansi -Wall -Wextra `sdl-config --cflags`

seratosthenes: seratosthenes.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o seratosthenes

