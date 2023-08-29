#ifndef RET2SPEC_H_
#define RET2SPEC_H_

#include "trex.h"
#include <stdlib.h>

// Recursion depth should be equal or greater than the RSB size, but not
// excessively high because of the possibility of stack overflow.
//
// Recent Intel CPUs increased their depth from 64 to 112, hopefully that
// doesn't cause issues on older CPUs.
#define RECURSION_DEPTH 128

// Modular function pointers that provide different functionality in the
// same-address-space and cross-address-space version. We use this design,
// because the same-address-space version works everywhere including on systems
// that do not allow forced pinning of processes to cores (e.g. MacOS), the
// cross-address-space version is Linux only.
extern void (*return_true_base_case)(void);
extern void (*return_false_base_case)(void);

// Global variables used to avoid passing parameters through recursive function
// calls. Since we flush whole stack frames from the cache, it is important not
// to store on stack any data that might be affected by being flushed from
// cache.
extern size_t current_offset;
extern struct page *oracle_ptr;

bool returns_false(int counter);
char ret2spec_leak_byte(void);

#endif // RET2SPEC_H_
