#include "plot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#define PRIME_WIDTH 600
#define PRIME_COUNT PRIME_WIDTH*PRIME_WIDTH

char *prime = NULL;

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

int main(int argc, char **argv) {
	int i, j;
	setPlotDimensions(PRIME_WIDTH, PRIME_WIDTH);
	plot_CheckState();
	clearPlot();

	generatePrimes();

	/*
	for(i = 2; i < PRIME_COUNT; ++i)
		if(prime[i])
			printf("%d ", i);
	printf("\n");
	*/

	glColor3f(0.0f, 0.0f, 1.0f);
	glPointSize(1);
	glBegin(GL_POINTS);
	for(i = 0; i < PRIME_WIDTH; ++i)
		for(j = 0; j < PRIME_WIDTH; ++j)
			if(prime[i*PRIME_WIDTH + j])
				glVertex2f(i, j);
	glEnd();
	SDL_GL_SwapBuffers();

	/*plotDelay(3000);*/
}

