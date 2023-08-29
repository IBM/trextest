#ifndef FAULTS_H_
#define FAULTS_H_

#include <trex.h>
#include <signal.h>

bool run_with_fault_handler(int signum, void (*inner)(void));

#endif // FAULTS_H_
