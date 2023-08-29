#include "faults.h"

#include <signal.h>
#include <stdio.h>

bool ran_body;

void segfault(void)
{
	ran_body = true;
	raise(SIGSEGV);
}

void dont_fault(void)
{
	ran_body = true;
}

bool test_segfault(void)
{
	bool pass = true;
	bool saw_fault = false;
	ran_body = false;

	saw_fault = run_with_fault_handler(SIGSEGV, segfault);
	if (!ran_body) {
		fprintf(stderr, "Didn't run expected function.\n");
		pass = false;
	}

	if (!saw_fault) {
		fprintf(stderr, "Didn't see expected fault.\n");
		pass = false;
	}

	return pass;
}

bool test_dont_fault(void)
{
	bool pass = true;
	bool saw_fault;

	ran_body = false;

	saw_fault = run_with_fault_handler(SIGSEGV, dont_fault);
	if (!ran_body) {
		fprintf(stderr, "Didn't run expected function.\n");
		pass = false;
	}

	if (saw_fault) {
		fprintf(stderr, "Saw expected fault.\n");
		pass = false;
	}

	return pass;
}

int main(int __unused argc, char __unused **argv)
{
	bool pass = true;

	// Run this test twice to check that we reset signal masks correctly.
	pass = pass && test_segfault();
	pass = pass && test_segfault();

	pass = pass && test_dont_fault();

	printf("%s\n", pass ? "pass" : "fail");

	return !pass;
}
