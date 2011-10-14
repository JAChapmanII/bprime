#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

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
	for(i = PRIME_COUNT; i > 0; --i)
		if(isPrime(prime, i))
			break;
	printf("Biggest prime: %ld\n", i);

	return 0;
}

void *crossOut(void *args);
/* this is hard-coded in generatePrimes, anyway */
size_t i = 0;
#define THREAD_COUNT 4
size_t generatePrimes() { /* {{{ */
	size_t pCount = 0, args[THREAD_COUNT << 1], m = sqrt(PRIME_COUNT) + 1;
	pthread_t threads[THREAD_COUNT];
	if(prime != NULL)
		return -2;

	prime = malloc(MEM_REQUIREMENT);
	if(!prime) {
		fprintf(stderr, "Could not allocate enough memory\n");
		exit(1);
	}
	memset(prime, 0xff, MEM_REQUIREMENT);

	printf("PRIME_COUNT: %ld\n", PRIME_COUNT);

	args[0] = 0;
	args[4] = (PRIME_COUNT >> 4) << 3;

	args[2] = (args[4] >> 4) << 3;
	args[6] = args[4] + ((args[4] >> 4) << 3);

	args[1] = args[2];
	args[3] = args[4];
	args[5] = args[6];
	args[7] = PRIME_COUNT;

	printf("t1: [%ld, %ld]\n", args[0], args[1]);
	printf("t2: [%ld, %ld]\n", args[2], args[3]);
	printf("t3: [%ld, %ld]\n", args[4], args[5]);
	printf("t4: [%ld, %ld]\n", args[6], args[7]);
	printf("m: %ld\n", m);

	for(i = 2; i < m; ++i) {
		if(i % 10000 == 0) {
			printf("%ld...\n", i);
			fflush(stdout);
		}
		if(!isPrime(prime, i))
			continue;
		pCount++;

		if(pthread_create(&threads[0], NULL, crossOut, (void *)(&args[0]))) {
			fprintf(stderr, "thread creation 0 failed!\n");
			exit(1);
		}
		if(pthread_create(&threads[1], NULL, crossOut, (void *)(&args[2]))) {
			fprintf(stderr, "thread creation 1 failed!\n");
			exit(1);
		}
		if(pthread_create(&threads[2], NULL, crossOut, (void *)(&args[4]))) {
			fprintf(stderr, "thread creation 2 failed!\n");
			exit(1);
		}
		if(pthread_create(&threads[3], NULL, crossOut, (void *)(&args[6]))) {
			fprintf(stderr, "thread creation 3 failed!\n");
			exit(1);
		}

		if(pthread_join(threads[0], NULL)) {
			fprintf(stderr, "thread 0 join failed\n");
			exit(1);
		}
		if(pthread_join(threads[1], NULL)) {
			fprintf(stderr, "thread 1 join failed\n");
			exit(1);
		}
		if(pthread_join(threads[2], NULL)) {
			fprintf(stderr, "thread 2 join failed\n");
			exit(1);
		}
		if(pthread_join(threads[3], NULL)) {
			fprintf(stderr, "thread 3 join failed\n");
			exit(1);
		}
		/* TODO: destroy threads? */
	}
	if(m % 64 != 0)
		m = ((m + 64) >> 6) << 6;
	for(; i < m; ++i)
		if(isPrime(prime, i))
			pCount++;
	for(i >>= 6; i < PRIME_COUNT >> 6; ++i)
		pCount += countSet(((uint64_t *)prime)[i]);
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
		setNotPrime(prime, j);
	return NULL;
}

