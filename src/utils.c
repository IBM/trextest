#include "utils.h"

#include <stdlib.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

void pin_to_cpu(int cpu)
{
#ifdef linux
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(cpu, &set);
	int res = sched_setaffinity(getpid(), sizeof(set), &set);
	if (res != 0) {
		printf("CPU affinity setup failed.\n");
		exit(EXIT_FAILURE);
	}
#else
	printf("Can only pin to CPU %d on Linux!\n", cpu);
#endif
}

int u64cmp(const void *p1, const void *p2)
{
	uint64_t lhs = *(uint64_t *)p1;
	uint64_t rhs = *(uint64_t *)p2;

	if (lhs > rhs) {
		return 1;
	} else if (rhs > lhs) {
		return -1;
	} else {
		return 0;
	}
}

// from the GCC documentation
/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}
