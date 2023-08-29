#include "trex.h"
#include "faults.h"

#include <setjmp.h>

sigjmp_buf signal_handler_jmpbuf;

void signal_handler(int __unused signal, siginfo_t __unused *info,
		    void __unused *ucontext)
{
	siglongjmp(signal_handler_jmpbuf, 1);
}

bool run_with_fault_handler(int signum, void (*inner)(void))
{
	struct sigaction oldsa, sa = { .sa_sigaction = signal_handler };
	bool handled_fault = true;

	// This sets the signal handler for the entire process.
	sigaction(signum, &sa, &oldsa);

	if (sigsetjmp(signal_handler_jmpbuf, 1) == 0) {
		inner();
		handled_fault = false;
	}

	sigaction(signum, &oldsa, NULL);

	return handled_fault;
}
