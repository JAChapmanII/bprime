#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#define PRIME_WIDTH  1920
#define PRIME_HEIGHT 1080
#define PRIME_COUNT PRIME_WIDTH*PRIME_HEIGHT
#define TORODIAL_WORLD 1

char *prime = NULL;
void generatePrimes();

SDL_Surface *screen;
void initSDL();

int main(int argc, char **argv) {
	int x = 0, y = 0;
	Uint32 on, off, *bufp;

	initSDL();

	generatePrimes();

	/*
	for(i = 2; i < PRIME_COUNT; ++i)
		if(prime[i])
			printf("%d ", i);
	printf("\n");
	*/

	on = SDL_MapRGB(screen->format, 0, 100, 255);
	off = SDL_MapRGB(screen->format, 255, 255, 255);

	if(screen->format->BytesPerPixel != 4) {
		fprintf(stderr, "Screen not 32-bpp\n");
		return 1;
	}


	if(SDL_MUSTLOCK(screen)) {
		if(SDL_LockSurface(screen) < 0) {
			fprintf(stderr, "Required to lock screen, but can't\n");
			return 1;
		}
	}
	for(x = 0; x < PRIME_WIDTH; ++x) {
		for(y = 0; y < PRIME_HEIGHT; ++y) {
			bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
			if(prime[y*PRIME_WIDTH + x]) {
				*bufp = on;
			} else {
				*bufp = off;
			}
		}
	}
	if(SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
	SDL_UpdateRect(screen, 0, 0, PRIME_WIDTH, PRIME_HEIGHT);

	SDL_SaveBMP(screen, "out.bmp");
	SDL_Delay(3000);
}


void initSDL() {
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
}

void generatePrimes() {
	int i, j;
	if(prime != NULL)
		return;

	prime = malloc(PRIME_COUNT);
	memset(prime, 1, PRIME_COUNT);

	for(i = 2; i < PRIME_COUNT; ++i) {
		if(!prime[i])
			continue;
		for(j = i << 1; j < PRIME_COUNT; j += i)
			prime[j] = 0;
	}
}


