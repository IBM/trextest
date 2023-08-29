#include "trex.h"
#include "timing_array.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>

int timing_array_test(void)
{
	struct timing_array *ta = ta_new();

	int attempts = 100000;
	int successes = 0;
	int false_positives = 0;
	int previous_element = -1;

	printf("Cached read latency threshold is %lu\n", ta->latency_threshold);

	for (int n = 0; n < attempts; n++) {
		unsigned char element = rand() & 0xff;
		ta_flush(ta);
		force_read(ta->entries[element]);

		int found = ta_find_first_cached_index(ta);
		if (found == element) {
			successes++;
		} else if (found != -1) {
			printf("False positive, found %d instead of %d\n", found,
			       element);
			printf("Previous value was %d\n", previous_element);
			false_positives++;
		}

		previous_element = element;
	}

	ta_free(ta);

	printf("Found cached element on the first try %d of %d times.\n", successes,
	       attempts);
	printf("False positives: %d\n", false_positives);

	return successes;
}

int main(int argc __unused, char **argv __unused)
{
	trex_calibrate();
	init_timer();
	return timing_array_test();
}
