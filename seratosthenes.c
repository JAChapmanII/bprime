#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#define PRIME_WIDTH  1001
#define PRIME_HEIGHT 1001
#define PRIME_COUNT PRIME_WIDTH*PRIME_HEIGHT
#define TORODIAL_WORLD 1

char *prime = NULL, *new = NULL, *tmp;
void generatePrimes();
int stepLife();

SDL_Surface *screen;
Uint32 pOn, pOff;
void initSDL();
void drawPrimeGrid();

int main(int argc, char **argv) {
	int i, tAN;

	initSDL();
	if(screen->format->BytesPerPixel != 4) {
		fprintf(stderr, "Screen not 32-bpp\n");
		return 1;
	}
	printf("Initialized SDL...\n");

	pOn  = SDL_MapRGB(screen->format,   0, 100, 255);
	pOff = SDL_MapRGB(screen->format, 255, 255, 255);
	printf("Constructed colors...\n");

	generatePrimes();
	printf("Generated initial prime set...\n");

	drawPrimeGrid();
	SDL_Delay(1000);

	for(i = 1; i > 0; ++i) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_RETURN: case SDLK_ESCAPE:
							i = -1;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
		if(i <= 0)
			break;
		drawPrimeGrid();
		SDL_Delay(1000);
		tAN = stepLife();
		if(tAN == 0)
			i = -1;
	}
	printf("Broken from game loop\n");

	SDL_SaveBMP(screen, "out.bmp");
	printf("Saved final output\n");
}


void generatePrimes() { /* {{{ */
	int i, j;
	if(prime != NULL)
		return;

	prime = malloc(PRIME_COUNT);
	new   = malloc(PRIME_COUNT);
	if(!prime || !new) {
		fprintf(stderr, "Could not allocate enough memory\n");
		exit(1);
	}
	memset(prime, 1, PRIME_COUNT);

	for(i = 2; i < PRIME_COUNT; ++i) {
		if(!prime[i])
			continue;
		for(j = i << 1; j < PRIME_COUNT; j += i)
			prime[j] = 0;
	}
} /* }}} */
int stepLife() { /* {{{ */
	int aliveCount, x, y, cx, cy, totalAliveNow = 0;
	for(x = 0; x < PRIME_WIDTH; ++x) {
		for(y = 0; y < PRIME_HEIGHT; ++y) {
			aliveCount = 0;

			/* Up neighbor */
			cx = x;
			cy = y - 1;
			if(TORODIAL_WORLD && cy < 0)
				cy = PRIME_HEIGHT - 1;
			if(cy >= 0)
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Down neighbor */
			cx = x;
			cy = y + 1;
			if(TORODIAL_WORLD && cy >= PRIME_HEIGHT)
				cy = 0;
			if(cy < PRIME_HEIGHT)
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Left neighbor */
			cx = x - 1;
			cy = y;
			if(TORODIAL_WORLD && cx < 0)
				cx = PRIME_WIDTH - 1;
			if(cx >= 0)
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Right neighbor */
			cx = x + 1;
			cy = y;
			if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
				cx = 0;
			if(cx < PRIME_WIDTH)
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Up left neighbor */
			cx = x - 1;
			cy = y - 1;
			if(TORODIAL_WORLD && cx < 0)
				cx = PRIME_WIDTH - 1;
			if(TORODIAL_WORLD && cy < 0)
				cy = PRIME_HEIGHT - 1;
			if((cx >= 0) && (cy >= 0))
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Up right neighbor */
			cx = x + 1;
			cy = y - 1;
			if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
				cx = 0;
			if(TORODIAL_WORLD && cy < 0)
				cy = PRIME_HEIGHT - 1;
			if((cx < PRIME_HEIGHT) && (cy >= 0))
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Down left neighbor */
			cx = x - 1;
			cy = y + 1;
			if(TORODIAL_WORLD && cx < 0)
				cx = PRIME_WIDTH - 1;
			if(TORODIAL_WORLD && cy >= PRIME_HEIGHT)
				cy = 0;
			if((cx >= 0) && (cy < PRIME_HEIGHT))
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			/* Down right neighbor */
			cx = x + 1;
			cy = y + 1;
			if(TORODIAL_WORLD && cx >= PRIME_WIDTH)
				cx = 0;
			if(TORODIAL_WORLD && cy >= PRIME_HEIGHT)
				cy = 0;
			if((cx < PRIME_WIDTH) && (cy < PRIME_HEIGHT))
				aliveCount += prime[cy*PRIME_WIDTH + cx];

			new[y*PRIME_WIDTH + x] = 0;
			if(prime[y*PRIME_WIDTH + x] && (aliveCount == 2) || (aliveCount == 3))
				new[y*PRIME_WIDTH + x] = 1, totalAliveNow++;
			if(!prime[y*PRIME_WIDTH + x] && (aliveCount == 3))
				new[y*PRIME_WIDTH + x] = 1, totalAliveNow++;
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
	int x, y;
	Uint32 *bufp;

	if(SDL_MUSTLOCK(screen)) {
		if(SDL_LockSurface(screen) < 0) {
			fprintf(stderr, "Required to lock screen, but can't\n");
			exit(1);
		}
	}
	for(x = 0; x < PRIME_WIDTH; ++x) {
		for(y = 0; y < PRIME_HEIGHT; ++y) {
			bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
			*bufp = (prime[y*PRIME_WIDTH + x]) ? pOn : pOff;
		}
	}
	if(SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
	SDL_UpdateRect(screen, 0, 0, PRIME_WIDTH, PRIME_HEIGHT);
} /* }}} */

