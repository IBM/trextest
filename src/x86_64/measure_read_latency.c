#include "timer.h"

/*
 * Use intel syntax for inline assembly. This in specified in the meson build file.
 */

#ifdef USE_TIMER_THREAD
uint64_t measure_read_latency(const void *addr)
{
	uint64_t time;
	__asm__ volatile(
		// Full memory and speculation barrier. See docs/fencing.md for details.
		"lfence\n"
		"mfence\n"

		// r8 = timer_thread_time
		"mov r8, [%[counter]]\n"

		// Finish reading the timestamp before starting the read.
		"lfence\n"

		// Read *rdi.
		"mov al, byte ptr [%[addr]]\n"

		// Finish the read before reading the timestamp again. LFENCE suffices here
		// because it serializes the instruction stream *and* waits for load
		// operations to complete.
		"lfence\n"

		// rax = timer_thread_time
		"mov rax, [%[counter]]\n"

		// rax -= r8
		"sub rax, r8\n"
		: "=&a"(time)
		: [addr] "r"(addr), [counter] "r"(&timer_thread_time)
		: "r8", "memory");
	return time;
}
#else
uint64_t measure_read_latency(const void *addr)
{
	uint64_t time;
	__asm__ volatile(
		// Full memory and speculation barrier. See docs/fencing.md for details.
		"lfence\n"
		"mfence\n"

		// edx:eax = <time-stamp counter>
		// RDTSC: https://cpu.fyi/d/484#G7.432796
		"rdtsc\n"

		// rax = edx:eax
		"shl rdx, 32\n"
		"or rax, rdx\n"

		// r8 = rax
		"mov r8, rax\n"

		// Finish reading the timestamp before starting the read.
		"lfence\n"

		// Read *rdi.
		"mov al, byte ptr [%[addr]]\n"

		// Finish the read before reading the timestamp again. LFENCE suffices here
		// because it serializes the instruction stream *and* waits for load
		// operations to complete.
		"lfence\n"

		// edx:eax = <time-stamp counter>
		"rdtsc\n"

		// rax = edx:eax
		"shl rdx, 32\n"
		"or rax, rdx\n"

		// rax -= r8
		"sub rax, r8\n"
		: "=&a"(time)
		: [addr] "r"(addr)
		: "rdx", "r8", "memory");
	return time;
}
#endif
