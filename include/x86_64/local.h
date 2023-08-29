#ifndef LOCAL_H_
#define LOCAL_H_

#include "trex.h"

#include <x86intrin.h>

#define DEFAULT_PAGE_SIZE 4096
#define DEFAULT_CACHELINE_SIZE 64

inline TREX_ALWAYS_INLINE void barrier_nospec(void)
{
	_mm_mfence();
	_mm_lfence();
}

inline TREX_ALWAYS_INLINE void flush_cacheline_nobarrier(const void *addr)
{
	_mm_clflush(addr);
}

#endif // LOCAL_H_
