#ifndef UTILS_H_
#define UTILS_H_

#include "trex.h"
#include <sys/time.h>

void pin_to_cpu(int cpu);
int u64cmp(const void *, const void *);
int timeval_subtract(struct timeval *, struct timeval *, struct timeval *);

#endif // UTILS_H_
