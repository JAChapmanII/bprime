#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL/SDL.h>

uint32_t PRIME_WIDTH;
uint32_t PRIME_HEIGHT;
uint32_t PRIME_COUNT;
uint32_t MEM_REQUIREMENT;
char TORODIAL_WORLD;

void setupConstants(uint32_t width, uint32_t heigth, char torodial) {
	PRIME_WIDTH  = width;
	PRIME_HEIGHT = heigth;
	PRIME_COUNT  = (PRIME_WIDTH*PRIME_HEIGHT);
	MEM_REQUIREMENT = ((PRIME_COUNT + 7) >> 3);
	TORODIAL_WORLD = torodial;
}

char *prime = NULL, *new = NULL, *tmp;

/* Functions to check/set/clear "prime"-ness {{{ */
int isPrime(char *p, int x) {
	return (p[x >> 3] & (0x1 << (x % 8))) >> (x % 8);
}
void setPrime(char *p, int x) {
	p[x >> 3] |= (0x1 << (x % 8));
}
void setNotPrime(char *p, int x) {
	if(isPrime(p, x))
		p[x >> 3] -= (0x1 << (x % 8));
} /* }}} */

int generatePrimes();
void writePrimes();
int stepLife();

SDL_Surface *screen;
Uint32 pOn, pOff;
void initSDL();
void drawPrimeGrid();

int main(int argc, char **argv) {
	int i, tAN, pCount, width, height, torodial;
	width = height = 768;
	torodial = 1;

	/* Parse arguments as width, height, and not torodial {{{ */
	if(argc > 1) {
		width = height = atoi(argv[1]);
		if(width <= 0)
			width = height = 768;
		if(argc > 2) {
			height = atoi(argv[2]);
			if(height <= 0)
				height = 768;
			if(argc > 3) {
				torodial = 0;
			}
		}
	} /* }}} */

	setupConstants(width, height, torodial);

	initSDL();
	if(screen->format->BytesPerPixel != 4) {
		fprintf(stderr, "Screen not 32-bpp\n");
		return 1;
	}
	printf("Initialized SDL...\n");

	pOn  = SDL_MapRGB(screen->format,   0, 100, 255);
	pOff = SDL_MapRGB(screen->format, 255, 255, 255);
	printf("Constructed colors...\n");

	pCount = generatePrimes();
	printf("Generated initial prime set...\n");
	printf("%d out of %d numbers were prime\n", pCount, PRIME_COUNT);

	writePrimes();
	printf("Saved prime grid to primes.rle\n");

	drawPrimeGrid();
	SDL_Delay(1000);

	for(i = 1; i > 0; ++i) {
		/* See if enter or escape was pressed {{{ */
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_RETURN: case SDLK_ESCAPE:
							printf("We recieved key input saying to stop.\n");
							i = -1;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		} /* }}} */
		/* If enter/escape was pressed, abort */
		if(i <= 0)
			break;
		drawPrimeGrid();
		/*SDL_Delay(333);*/
		tAN = stepLife();
		if(tAN == 0) {
			printf("All life is dead.\n");
			i = -1;
		}
	}
	printf("Broken from game loop\n");

	SDL_SaveBMP(screen, "out.bmp");
	printf("Saved final output\n");

	return 0;
}


int generatePrimes() { /* {{{ */
	uint32_t i, j, pCount = 0;
	if(prime != NULL)
		return -2;

	prime = malloc(MEM_REQUIREMENT);
	if(!prime) {
		fprintf(stderr, "Could not allocate enough memory\n");
		exit(1);
	}
	for(i = 0; i < PRIME_COUNT; ++i)
		setPrime(prime, i);

	for(i = 2; i < PRIME_COUNT; ++i) {
		if(!isPrime(prime, i))
			continue;
		pCount++;
		for(j = i * 2; j < PRIME_COUNT; j += i)
			setNotPrime(prime, j);
	}
	return pCount;
} /* }}} */
void writePrimes() { /* {{{ */
	uint32_t x, y, cnt;
	char on;
	FILE *f = fopen("primes.rle", "w");
	if(!f) {
		fprintf(stderr, "Could not open primes.rle for writing\n");
		return;
	}

	fprintf(f, "x = %d, y = %d, rule = B3/S23\n", PRIME_WIDTH, PRIME_HEIGHT);
	for(y = 0; y < PRIME_HEIGHT; ++y) {
		for(x = 0; x < PRIME_WIDTH; ++x) {
			on = isPrime(prime, y*PRIME_WIDTH + x);
			for(cnt = 0; (x < PRIME_WIDTH) && (isPrime(prime, y*PRIME_WIDTH + x) == on); ++x)
				cnt++;
			if((x != PRIME_WIDTH - 1) || (isPrime(prime, y*PRIME_WIDTH + PRIME_WIDTH) != on))
				--x;
			if(cnt > 1)
				fprintf(f, "%d%c", cnt, (on) ? 'o' : 'b');
			else
				fprintf(f, "%c", (on) ? 'o' : 'b');
		}
		fprintf(f, "$");
	}
	fprintf(f, "!\n");

	fclose(f);
} /* }}} */
int stepLife() { /* {{{ */
	uint32_t x, y, cx, cy, totalAliveNow = 0;
	char aliveCount;

	if(!new) {
		new = malloc(MEM_REQUIREMENT);
		if(!new) {
			fprintf(stderr, "Could not allocate enough memory to simulate!\n");
			exit(1);
		}
	}

	for(x = 0; x < PRIME_WIDTH; ++x) {
		for(y = 0; y < PRIME_HEIGHT; ++y) {
			aliveCount = 0;

			cx = x;
			/* Up neighbor */
			cy = y - 1;
			if(TORODIAL_WORLD && y == 0)
				cy = PRIME_HEIGHT - 1;
			if(TORODIAL_WORLD || y != 0)
				aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

			/* Down neighbor */
			cy = y + 1;
			if(TORODIAL_WORLD && cy >= PRIME_HEIGHT)
				cy = 0;
			if(cy < PRIME_HEIGHT)
				aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

			cy = y;
			/* Left neighbor */
			cx = x - 1;
			if(TORODIAL_WORLD && x == 0)
				cx = PRIME_WIDTH - 1;
			if(TORODIAL_WORLD || x != 0)
				aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

			/* Right neighbor */
			cx = x + 1;
			if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
				cx = 0;
			if(cx < PRIME_WIDTH)
				aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

			cy = y - 1;
			if(TORODIAL_WORLD && y == 0)
				cy = PRIME_HEIGHT - 1;
			if(TORODIAL_WORLD || y != 0) {
				/* Up left neighbor */
				cx = x - 1;
				if(TORODIAL_WORLD && x == 0)
					cx = PRIME_WIDTH - 1;
				if(TORODIAL_WORLD || x != 0)
					aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

				/* Up right neighbor */
				cx = x + 1;
				if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
					cx = 0;
				if(cx < PRIME_WIDTH)
					aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);
			}

			cy = y + 1;
			if(TORODIAL_WORLD && cy >= PRIME_HEIGHT)
				cy = 0;
			if(cy < PRIME_HEIGHT) {
				/* Down left neighbor */
				cx = x - 1;
				if(TORODIAL_WORLD && x == 0)
					cx = PRIME_WIDTH - 1;
				if(TORODIAL_WORLD || x != 0)
					aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);

				/* Down right neighbor */
				cx = x + 1;
				if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
					cx = 0;
				if(cx < PRIME_WIDTH)
					aliveCount += isPrime(prime, cy*PRIME_WIDTH + cx);
			}

			setNotPrime(new, y*PRIME_WIDTH + x);
			if(isPrime(prime, y*PRIME_WIDTH + x) &&
					((aliveCount == 2) || (aliveCount == 3)))
				setPrime(new, y*PRIME_WIDTH + x);
			if(!isPrime(prime, y*PRIME_WIDTH + x) && (aliveCount == 3))
				setPrime(new, y*PRIME_WIDTH + x);
			if(isPrime(new, y*PRIME_WIDTH + x))
				totalAliveNow++;
		}
	}
	tmp = prime;
	prime = new;
	new = tmp;
	return totalAliveNow;
} /* }}} */

void initSDL() { /* {{{ */
	const SDL_VideoInfo *videoInfo;
	int videoFlags;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	videoInfo = SDL_GetVideoInfo();
	if(!videoInfo) {
		fprintf(stderr, "Can't query video info: %s\n", SDL_GetError());
		exit(1);
	}

	videoFlags = SDL_HWPALETTE;
	/*videoFlags |= SDL_RESIZABLE; TODO */

	if(videoInfo->hw_available)
		videoFlags |= SDL_HWSURFACE;
	else
		videoFlags |= SDL_SWSURFACE;

	if(videoInfo->blit_hw)
		videoFlags |= SDL_HWACCEL;

	screen = SDL_SetVideoMode(PRIME_WIDTH, PRIME_HEIGHT, 32, videoFlags);
	if(screen == NULL) {
		fprintf(stderr, "Unable to create plot screen: %s\n", SDL_GetError());
		exit(1);
	}
} /* }}} */
void drawPrimeGrid() { /* {{{ */
	uint32_t x, y, *bufp;

	if(SDL_MUSTLOCK(screen)) {
		if(SDL_LockSurface(screen) < 0) {
			fprintf(stderr, "Required to lock screen, but can't\n");
			exit(1);
		}
	}
	for(x = 0; x < PRIME_WIDTH; ++x) {
		for(y = 0; y < PRIME_HEIGHT; ++y) {
			bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
			*bufp = (isPrime(prime, y*PRIME_WIDTH + x)) ? pOn : pOff;
		}
	}
	if(SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
	SDL_UpdateRect(screen, 0, 0, PRIME_WIDTH, PRIME_HEIGHT);
} /* }}} */

