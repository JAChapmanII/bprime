#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

size_t PRIME_WIDTH, PRIME_HEIGHT, PRIME_COUNT, MEM_REQUIREMENT;

void setupConstants(size_t width, size_t height) {
	PRIME_WIDTH  = width;
	PRIME_HEIGHT = height;
	PRIME_COUNT = width * height;
	MEM_REQUIREMENT = ((PRIME_COUNT + 7) >> 3);
}

char *prime = NULL;

/* Functions to check/set/clear "prime"-ness {{{ */
size_t isPrime(char *p, size_t x) {
	return (p[x >> 3] & (0x1 << (x % 8))) >> (x % 8);
}
void setPrime(char *p, size_t x) {
	p[x >> 3] |= (0x1 << (x % 8));
}
void setNotPrime(char *p, size_t x) {
	if(isPrime(p, x))
		p[x >> 3] -= (0x1 << (x % 8));
} /* }}} */

size_t generatePrimes();

int main(int argc, char **argv) {
	size_t pCount, width, height = width = 512, i;

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
	printf("%ld out of %ld numbers were prime\n", pCount, PRIME_COUNT);
	for(i = width*height; i > 0; --i)
		if(isPrime(prime, i))
			break;
	printf("Biggest prime: %ld\n", i);

	return 0;
}

size_t generatePrimes() { /* {{{ */
	size_t i, j, pCount = 0;
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

