#ifndef LOCAL_H_
#define LOCAL_H_

#include "trex.h"

// We need to make sure we can always spread across pages, so better to use 64k than 4k.
#define DEFAULT_PAGE_SIZE 65536
#define DEFAULT_CACHELINE_SIZE 128

inline TREX_ALWAYS_INLINE void barrier_nospec(void)
{
	__asm__ volatile("hwsync\n"
			 "ori 31,31,0\n"
			 :
			 :
			 : "memory");
}

inline TREX_ALWAYS_INLINE void flush_cacheline_nobarrier(const void *address)
{
	// "data cache block flush" with L=0 to invalidate the cache block across all
	// processors. https://cpu.fyi/d/a48#G19.1156482
	__asm__ volatile("dcbf 0, %0\n" : : "r"(address) : "memory");
}

#endif // LOCAL_H_
