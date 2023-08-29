#include "timer.h"

#ifdef USE_TIMER_THREAD
uint64_t measure_read_latency(const void *addr)
{
	uint64_t time;
	__asm__ volatile("dsb sy\n"
			 "isb\n"
			 "ldr x8, [%[counter]]\n"
			 "isb\n"
			 "ldr x10, [%[addr]]\n"
			 "isb\n"
			 "ldr x9, [%[counter]]\n"
			 "sub %0, x9, x8\n"
			 : "=r"(time)
			 : [counter] "r"(&timer_thread_time), [addr] "r"(addr)
			 : "x8", "x9", "x10", "memory");
	return time;
}
#else
#error aarch64 requires USE_TIMER_THREAD to be defined.
#endif // USE_TIMER_THREAD
