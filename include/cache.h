#ifndef CACHE_H_
#define CACHE_H_

#include "trex.h"
#include "local.h"

inline TREX_ALWAYS_INLINE void flush_cacheline(void *addr)
{
	flush_cacheline_nobarrier(addr);
	barrier_nospec();
}

const void *next_cacheline(const void *addr);
void flush_cache_range(const void *start, const void *end);

#endif // CACHE_H_
