#include "trex.h"
#include "timer.h"
#include <sched.h>
#include <stdio.h>
#include <pthread.h>

#ifdef USE_TIMER_THREAD
static pthread_t timer;
volatile uint64_t timer_thread_time;

void *timer_thread(void *arg __unused)
{
	printf("Timer thread started\n");
	while (1) {
		timer_thread_time += 1;
	}
	return 0;
}

void init_timer(void)
{
	if (!timer) {
		pthread_create(&timer, NULL, timer_thread, NULL);

		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(1, &cpuset);
		int ret = pthread_setaffinity_np(timer, sizeof(cpu_set_t), &cpuset);
		printf("Pinned timing thread to cpu 1: %d\n", ret);
	}
}
#else
void init_timer(void)
{
}
#endif
