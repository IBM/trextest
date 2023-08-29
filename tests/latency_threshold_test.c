#include "trex.h"
#include "timing_array.h"
#include "timer.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc __unused, char **argv __unused)
{
	init_timer();

	for (int i = 0; i < 10; i++) {
		struct timing_array *ta = ta_new();
		printf("Cached read latency threshold is %lu\n", ta->latency_threshold);
		uint64_t latency_threshold = ta->latency_threshold;
		ta_free(ta);
		if (latency_threshold == 0) {
#ifdef USE_TIMER_THREAD
			printf("Timer thread value: %lu\n", timer_thread_time);
#endif
		}
	}

	return 0;
}
