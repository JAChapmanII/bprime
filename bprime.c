#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

size_t PRIME_COUNT, MEM_REQUIREMENT;
char *prime = NULL;

/* Functions to check/set/clear "prime"-ness {{{ */
size_t isPrime(char *p, size_t x) {
	x = (x >> 1) - 1;
	return (p[x >> 3] & (0x1 << (x % 8))) >> (x % 8);
}
void setPrime(char *p, size_t x) {
	x = (x >> 1) - 1;
	p[x >> 3] |= (0x1 << (x % 8));
}
void setNotPrime(char *p, size_t x) {
	if(isPrime(p, x)) {
		x = (x >> 1) - 1;
		p[x >> 3] -= (0x1 << (x % 8));
	}
} /* }}} */

/* */
#define m1  0x5555555555555555
#define m2  0x3333333333333333
#define m4  0x0f0f0f0f0f0f0f0f
#define h01 0x0101010101010101

size_t countSet(uint64_t x) {
	x -= (x >> 1) & m1;
	x = (x & m2) + ((x >> 2) & m2);
	x = (x + (x >> 4)) & m4;
	return (x * h01) >>56;
}

size_t generatePrimes();

int main(int argc, char **argv) {
	size_t pCount, count = 512, i;

	/* Parse argument as square root of PRIME_COUNT {{{ */
	if(argc > 1) {
		count = atoi(argv[1]);
		if(count <= 0)
			count = 512;
	} /* }}} */
	PRIME_COUNT = count * count;
	/* We add 7 so we always round up to the nearest byte, then we divide by
	 * 	8 to go from bits to bytes
	 * 	2 to go from standard sieve to the 1st extension
	 */
	MEM_REQUIREMENT = (((PRIME_COUNT + 7) >> 3) + 1) >> 1;
	/* Align to 8 bytes so our uint64_t thing works */
	/*MEM_REQUIREMENT = ((MEM_REQUIREMENT + 7) >> 3) << 3;*/
	printf("Prime count: %ld, bytes needed: %ld\n", PRIME_COUNT, MEM_REQUIREMENT);

	pCount = generatePrimes();
	printf("Generated initial prime set...\n");
	printf("%ld out of %ld numbers were prime\n", pCount, PRIME_COUNT);
	for(i = PRIME_COUNT - 1; i > 2; i -= 2)
		if(isPrime(prime, i))
			break;
	printf("Biggest prime: %ld\n", i);

	pCount = 1;
	/*printf("2 ");*/
	for(i = 3; i < PRIME_COUNT; i += 2)
		if(isPrime(prime, i)) {
			/*printf("%ld ", i);*/
			pCount++;
		}
	printf("\n%ld\n", pCount);

	return 0;
}

void *crossOut(void *args);

size_t i = 0;
#define THREAD_COUNT 4
size_t generatePrimes() { /* {{{ */
	size_t pCount = 1, args[THREAD_COUNT << 1], m = sqrt(PRIME_COUNT) + 1, j, s;
	pthread_t threads[THREAD_COUNT];
	if(prime != NULL)
		return -2;

	prime = malloc(MEM_REQUIREMENT);
	if(!prime) {
		fprintf(stderr, "Could not allocate enough memory\n");
		exit(1);
	}
	memset(prime, 0xff, MEM_REQUIREMENT);

	/* byte align boundaries between threads */
	s = (((PRIME_COUNT / THREAD_COUNT + 7) >> 3) << 3);
	for(j = 0; j < THREAD_COUNT; ++j) {
		args[(j << 1) + 0] = j * s;
		args[(j << 1) + 1] = j * s + s;
		printf("t%ld: [%ld, %ld]\n", j, args[(j << 1)], args[(j << 1) + 1]);
	}
	args[0] = 0;
	args[(THREAD_COUNT << 1) - 1] = PRIME_COUNT;

	printf("m: %ld\n", m);

	for(i = 3; i < m; i += 2) {
		if(!isPrime(prime, i))
			continue;

		for(j = 0; j < THREAD_COUNT; ++j)
			if(pthread_create(&threads[j], NULL,
						crossOut, (void *)(&args[j << 1]))) {
				fprintf(stderr, "thread creation %ld failed!\n", j);
				exit(1);
			}

		for(j = 0; j < THREAD_COUNT; ++j)
			if(pthread_join(threads[j], NULL)) {
				fprintf(stderr, "thread %ld join failed\n", j);
				exit(1);
			}

		/* TODO: destroy threads? */
	}
	/* short method to sum up the the bits up to the last < 64b segment */
	for(i = 0; i < ((PRIME_COUNT - 2) >> 7); ++i) {
		printf("quick summing %ld to %ld\n", (i << 7) + 2, (i << 7) + 129);
		pCount += countSet(((uint64_t *)prime)[i]);
	}
	if(!i)
		i = 3;
	else
		i <<= 7, i += 3;
	for(; i < PRIME_COUNT; i += 2) {
		printf("Trying to count %ld\n", i);
		if(isPrime(prime, i))
			pCount++;
	}
	return pCount;
} /* }}} */

void *crossOut(void *args) {
	size_t j;
	if(((size_t *)args)[0] == 0) {
		j = i * i;
	} else {
		j = (((size_t *)args)[0] / i) * i;
		if(j < ((size_t *)args)[0])
			j += i;
	}
	if(j <= i)
		j += i;
	if(((size_t *)args)[1] < j)
		return NULL;
	for(; j < ((size_t *)args)[1]; j += i)
		if(j % 2)
			setNotPrime(prime, j);
	return NULL;
}

