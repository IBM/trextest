#include "trex.h"
#include "local.h"

#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

size_t page_size;
size_t cacheline_size;

void trex_calibrate(void)
{
	struct utsname buf;
	long result = -1;
#ifdef _SC_PAGE_SIZE
	result = sysconf(_SC_PAGE_SIZE);
#else
	errno = ENOSYS;
#endif
	if (result < 0) {
		fprintf(stderr,
			"Couldn't find page size: (%s), falling back to default\n",
			strerror(errno));
		fflush(stderr);
		page_size = DEFAULT_PAGE_SIZE;
	} else {
		page_size = result;
	}

#ifdef _SC_LEVEL1_DCACHE_LINESIZE
	result = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
#else
	result = -1;
	errno = ENOSYS;
#endif
	if (result < 0) {
		// TODO check /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
		fprintf(stderr,
			"Couldn't find cacheline size: (%s), falling back to default\n",
			strerror(errno));
		fflush(stderr);
		cacheline_size = DEFAULT_CACHELINE_SIZE;
	} else {
		cacheline_size = result;
	}

	if (uname(&buf)) {
		fprintf(stderr, "uname() failed: (%s), bailing out.\n", strerror(errno));
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	printf("trex on %s ready\nusing page size %lu cacheline size %lu\n\n",
	       buf.machine, page_size, cacheline_size);
	fflush(stdout);
}
