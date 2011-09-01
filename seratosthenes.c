#include "plot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRIME_COUNT 25
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
	int i;
	plot_CheckState();
	clearPlot();

	generatePrimes();

	for(i = 2; i < PRIME_COUNT; ++i)
		if(prime[i])
			printf("%d ", i);
	printf("\n");
	/*plotDelay(3000);*/
}



