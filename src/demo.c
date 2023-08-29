#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "timer.h"
#include "trex.h"
#include "demo.h"
#include "utils.h"
#include "timing_array.h"

char *public_data = "00000000000000000000000000";
char *private_data = "abcdefghijklmnopqrstuvwxyz";

pid_t parent_pid;
pid_t child_pid;
bool is_child = false;

static struct timeval start;

static char *map_data(unsigned long hint)
{
	char *p;

	if (hint)
		hint += 0x11111111111ul;

	p = mmap((void *)hint, 1ul << 30, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	assert(p != MAP_FAILED);

	p += ((unsigned long)p >> 16) & 0xffff;

	memset(p, 0, page_size);

	return p;
}

void trex_demo_init(bool is_ca, bool mmap)
{
#ifdef linux
	int i;

	if (mmap) {
		public_data = map_data(0);
		assert(public_data);

		private_data = map_data((unsigned long)public_data);
		assert(private_data);
		for (i = 0; i < SECRET_LEN; i++)
			private_data[i] = 'a' + i;
		private_data[SECRET_LEN] = '\0';
	}
#endif

	printf("public_data  @ %p\n", (void *)public_data);
	printf("private_data @ %p\n", (void *)private_data);

	trex_calibrate();

	// Initialise the timer thread. Does nothing if using a hardware timer
	init_timer();

	if (is_ca) {
		pin_to_cpu(0);
		parent_pid = getpid();
		child_pid = fork();
		is_child = child_pid == 0;

		if (is_child)
			return;
	}

	printf("Attempting to leak the secret...\n\n");
	fflush(stdout);

	// Record start time
	gettimeofday(&start, NULL);
}

void trex_demo_exit(int ret)
{
	struct timeval end, diff;

	if (is_child)
		exit(ret);

	gettimeofday(&end, NULL);
	timeval_subtract(&diff, &end, &start);
	printf("\nRun took %lu.%06lds\n", diff.tv_sec, diff.tv_usec);

	switch (ret) {
	case TREX_SUCCESS:
		printf("\nDone!\n");
		break;
	case TREX_COULDNT_LEAK:
		printf("\nCould not leak the secret.\n");
		break;
	case TREX_ERROR:
		printf("\ntrex had an error, quitting.\n");
		break;
	}
	fflush(stdout);

	exit(ret);
}
