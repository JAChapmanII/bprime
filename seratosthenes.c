#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
void writePrimes();

int main(int argc, char **argv) {
	int pCount, width, height = width = 512;

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

	writePrimes();
	printf("Saved prime grid to primes.rle\n");

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

