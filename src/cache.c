#include "trex.h"
#include "cache.h"

#include <stdint.h>

inline TREX_ALWAYS_INLINE const void *next_cacheline(const void *addr)
{
	return (void *)((((uintptr_t)addr) + cacheline_size) & ~(cacheline_size - 1));
}

void flush_cache_range(const void *start, const void *end)
{
	for (; start < end; start = next_cacheline(start)) {
		flush_cacheline_nobarrier(start);
	}
	barrier_nospec();
}
