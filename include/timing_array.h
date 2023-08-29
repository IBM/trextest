#ifndef TIMING_ARRAY_H_
#define TIMING_ARRAY_H_

#include "trex.h"
#include "cache.h"

#include <stdlib.h>
#include <stdint.h>

struct timing_array {
	char *array_ptr;
	// One entry for each byte value
	uint64_t *entries[256];
	uint64_t latency_threshold;
};

// Allocate a new timing array
struct timing_array *ta_new(void);

// Clean up a timing array
void ta_free(struct timing_array *ta);

// Get the memory location of a given element's actual value
int *ta_get(struct timing_array *ta, size_t index);

// Flush the whole timing array
void ta_flush(struct timing_array *ta);

// Find the first index in the array that's under the latency threshold.
// Returns -1 if none found.
int ta_find_first_cached_index(struct timing_array *ta);

// Find the first index in the array after the given index that's under the latency threshold.
// Returns -1 if none found.
int ta_find_first_cached_index_after(struct timing_array *ta, int index);

uint64_t ta_find_threshold(struct timing_array *ta);

#endif // TIMING_ARRAY_H_
