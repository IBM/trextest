#include "timer.h"

#ifdef USE_TIMER_THREAD
uint64_t measure_read_latency(const void *addr)
{
	uint64_t time;
	__asm__ volatile(
		// Full memory and speculation barrier. See docs/fencing.md for details.
		"hwsync\n"
		"ori 31,31,0\n"

		// The instruction to read the Time Base used to be "Move From Time Base"
		// (MFTB, [1]) but the Power manual now recommends "Move From Special Purpose
		// Register" (MFSPR). `MFTB n` is now a mnemonic for `MFSPR n, 268`.
		//
		// [1] MFTB: https://cpu.fyi/d/a48#G21.999352
		"ld 4, 0(%[counter])\n"
		"twi 0, 4, 0\n"
		"isync\n"

		// Finish reading Time Base before starting the read.
		// TODO update this comment
		// We only need to serialize the instruction stream and we don't need a
		// memory barrier, so ISYNC is good enough.
		"ori 31,31,0\n"

		// Read *r3.
		"lbz 5, 0(%[addr])\n"
		"twi 0,5,0\n"
		"isync\n"

		// Finish the read before reading Time Base again. This *does* require a
		// memory barrier.
		"ori 31,31,0\n"

		"ld %[time], 0(%[counter])\n"
		"sub %[time], %[time],4\n"
		: [time] "=r"(time)
		: [addr] "r"(addr), [counter] "r"(&timer_thread_time)
		: "4", "5", "memory");
	return time;
}
#else
uint64_t measure_read_latency(const void *addr)
{
	uint64_t time;
	__asm__ volatile(
		// Full memory and speculation barrier. See docs/fencing.md for details.
		"hwsync\n"
		"ori 31,31,0\n"

		// The instruction to read the Time Base used to be "Move From Time Base"
		// (MFTB, [1]) but the Power manual now recommends "Move From Special Purpose
		// Register" (MFSPR). `MFTB n` is now a mnemonic for `MFSPR n, 268`.
		//
		// [1] MFTB: https://cpu.fyi/d/a48#G21.999352
		"mfspr 4, 268\n"

		// Finish reading Time Base before starting the read.
		// TODO update this comment
		// We only need to serialize the instruction stream and we don't need a
		// memory barrier, so ISYNC is good enough.
		"ori 31,31,0\n"

		// Read *r3.
		"lbz 5, 0(%[addr])\n"
		"twi 0,5,0\n"
		"isync\n"

		// Finish the read before reading Time Base again. This *does* require a
		// memory barrier.
		"ori 31,31,0\n"

		"mfspr %[time], 268\n"
		"sub %[time],%[time],4\n"
		: [time] "=r"(time)
		: [addr] "r"(addr)
		: "4", "5", "memory");
	return time;
}
#endif
