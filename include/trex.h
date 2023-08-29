#ifndef TREX_H_
#define TREX_H_

#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>

#ifdef __x86_64__
#define TREX_X86_64
#endif

#ifdef __powerpc64__
#define TREX_POWER
#endif

#define TREX_ALWAYS_INLINE __attribute__((always_inline))
#define TREX_NEVER_INLINE __attribute__((noinline))
#define TREX_MAGIC 0xBAD70A57

#define __weak __attribute((weak))
#define __unused __attribute__((unused))

inline TREX_ALWAYS_INLINE void force_read(const void *p)
{
	(void)*(const volatile char *)(p);
}

extern size_t page_size;
extern size_t cacheline_size;

enum {
	TREX_SUCCESS = 0,
	TREX_COULDNT_LEAK,
	TREX_ERROR,
};

void trex_calibrate(void);

#endif // TREX_H_
