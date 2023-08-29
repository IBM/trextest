#include "trex.h"
#include "latency.h"
#include "timing_array.h"
#include "utils.h"
#include <sys/param.h>

static uint64_t threshold = TREX_MAGIC;

struct timing_array *ta_new(void)
{
	int i;
	struct timing_array *ta = malloc(sizeof(struct timing_array));
	size_t ta_entry_size = page_size + cacheline_size;
	size_t index;

	// Entries are a page and a cacheline long.
	// We allocate a contiguous block of memory to remove locality issues.
	// One entry for each byte value, with padding on each side.
	ta->array_ptr = malloc(ta_entry_size * 258);

	// Here, the entries we want to use are indexes [1:256] with 0 and 257 as padding.
	// We use an affine map with a fixed offset so the CPU can't get clever and prefetch
	// sequential entries in the array.
	for (i = 0; i < 256; i++) {
		index = (100 + i * 113) % 256;
		ta->entries[i] =
			(uint64_t *)(ta->array_ptr + (ta_entry_size * (index + 1)));
	}

	// Write to each element to eliminate the possibility of shared mappings
	for (i = 0; i < 256; i++) {
		*ta->entries[i] = TREX_MAGIC;
	}

	if (threshold == TREX_MAGIC)
		threshold = ta_find_threshold(ta);

	ta->latency_threshold = threshold;

	return ta;
}

void ta_free(struct timing_array *ta)
{
	free(ta->array_ptr);
	free(ta);
	ta = NULL;
}

void ta_flush(struct timing_array *ta)
{
	for (int i = 0; i < 256; i++) {
		flush_cacheline_nobarrier(ta->entries[i]);
	}

	barrier_nospec();
}

int ta_find_first_cached_index_after(struct timing_array *ta, int start_after)
{
	unsigned char index;
	uint64_t latency;
	size_t iterations, start, i;
	static uint64_t count = 0;

	if (start_after >= 256 || start_after < -1) {
		return -1;
	}

	start = (start_after + 1) % 256;
	iterations = start_after == -1 ? 256 : 255;

	for (i = 0; i < iterations; i++) {
		index = (start + i) % 256;
		latency = measure_read_latency(ta->entries[index]);
		if (latency <= ta->latency_threshold) {
			return index;
		}
	}

	// If we've been spinning for a while and haven't found a hit, try recalibrating
	// the latency threshold.  Even if the latency threshold hasn't changed, sometimes
	// accessing the whole array again can help performance.
	if (++count % 1000 == 0) {
		ta->latency_threshold = ta_find_threshold(ta);
		count = 0;
	}

	return -1;
}

int ta_find_first_cached_index(struct timing_array *ta)
{
	return ta_find_first_cached_index_after(ta, -1);
}

uint64_t ta_find_threshold(struct timing_array *ta)
{
	const int percentile = 10;
#define ITERATIONS 1000
	uint64_t max_read_latencies[ITERATIONS];
	uint64_t max_read_latency, latency;
	const int iterations = ITERATIONS;
	int i, n, index;

	for (n = 0; n < ITERATIONS; n++) {
		// Bring all elements into the cache.
		for (i = 0; i < 256; i++) {
			force_read(ta->entries[i]);
		}

		max_read_latency = 0;
		for (i = 0; i < 256; i++) {
			latency = measure_read_latency(ta->entries[i]);
			if (latency > max_read_latency)
				max_read_latency = latency;
		}

		max_read_latencies[n] = max_read_latency;
	}

	qsort(&max_read_latencies[0], ITERATIONS, sizeof(uint64_t), u64cmp);
	index = (percentile / 100.0) * (iterations - 1);

	threshold = max_read_latencies[index];

	return threshold;
}
