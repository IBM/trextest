#ifndef TIMER_THREAD_H_
#define TIMER_THREAD_H_

#include "trex.h"
#include <stdint.h>

void init_timer(void);

#ifdef USE_TIMER_THREAD

extern volatile uint64_t timer_thread_time;

#endif // USE_TIMER_THREAD

#endif // TIMER_THREAD_H_
