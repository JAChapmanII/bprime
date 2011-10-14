#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t PRIME_WIDTH, PRIME_HEIGHT, PRIME_COUNT, MEM_REQUIREMENT;

void setupConstants(uint32_t width, uint32_t height) {
	PRIME_WIDTH  = width;
	PRIME_HEIGHT = height;
	PRIME_COUNT = width * height;
	MEM_REQUIREMENT = ((PRIME_COUNT + 7) >> 3);
}

char *prime = NULL;

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

int main(int argc, char **argv) {
	int pCount, width, height = width = 512, i;

	/* Parse arguments as width, and height {{{ */
	if(argc > 1) {
		height = width = atoi(argv[1]);
		if(width <= 0)
			width = height = 512;
		if(argc > 2) {
			height = atoi(argv[2]);
			if(height <= 0)
				height = 512;
		}
	} /* }}} */

	setupConstants(width, height);

	pCount = generatePrimes();
	printf("Generated initial prime set...\n");
	printf("%d out of %d numbers were prime\n", pCount, PRIME_COUNT);
	for(i = width*height; i >= 0; --i)
		if(isPrime(prime, i))
			break;
	printf("Biggest prime: %d\n", i);

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
	memset(prime, 0xff, MEM_REQUIREMENT);

	for(i = 2; i < PRIME_COUNT; ++i) {
		if(!isPrime(prime, i))
			continue;
		pCount++;
		for(j = i * 2; j < PRIME_COUNT; j += i)
			setNotPrime(prime, j);
	}
	return pCount;
} /* }}} */

