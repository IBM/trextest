#ifndef DEMO_H_
#define DEMO_H_

#include "trex.h"

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

// Common shared code for demos.

#define SECRET_LEN 26

extern char *public_data;
extern char *private_data;

extern pid_t parent_pid;
extern pid_t child_pid;
extern bool is_child;

void trex_demo_init(bool is_ca, bool mmap);
void trex_demo_exit(int ret);

static inline void print_leak(char c, int index)
{
	int error;

	error = c - ('a' + index);

	printf("%c (%02hhx) error=%d\n", isprint(c) ? c : '?', (unsigned char)c, error);

	fflush(stdout);
}

#endif // DEMO_H_
