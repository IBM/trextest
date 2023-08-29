#ifndef LOCAL_H_
#define LOCAL_H_

#include "trex.h"

// We need to make sure we can always spread across pages, so better to use 64k than 4k.
#define DEFAULT_PAGE_SIZE 65536
#define DEFAULT_CACHELINE_SIZE 128

inline TREX_ALWAYS_INLINE void barrier_nospec(void)
{
	__asm__ volatile("dsb sy\n"
			 "isb\n"
			 :
			 :
			 : "memory");
}

inline TREX_ALWAYS_INLINE void flush_cacheline_nobarrier(const void *address)
{
	__asm__ volatile("dc civac, %0\n" : : "r"(address) : "memory");
}

#endif // LOCAL_H_
