LDFLAGS=-lm `sdl-config --libs` -lGL
CFLAGS=-pedantic -ansi -W -Wextra `sdl-config --cflags`

seratosthenes: seratosthenes.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o seratosthenes

