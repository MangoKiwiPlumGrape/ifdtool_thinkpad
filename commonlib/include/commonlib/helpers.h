/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Minimal vendor stub for commonlib/helpers.h
 * Extracted from coreboot src/commonlib/include/commonlib/helpers.h
 * Only the macros required by ifdtool are included here.
 *
 * Place this file at: commonlib/include/commonlib/helpers.h
 * (relative to the ifdtool source directory)
 */

#ifndef _COMMONLIB_HELPERS_H_
#define _COMMONLIB_HELPERS_H_

#include <stddef.h>
#include <commonlib/bsd/compiler.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef MIN
#define MIN(a, b) ({ \
	__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	_a < _b ? _a : _b; \
})
#endif

#ifndef MAX
#define MAX(a, b) ({ \
	__typeof__(a) _a = (a); \
	__typeof__(b) _b = (b); \
	_a > _b ? _a : _b; \
})
#endif

#ifndef IS_POWER_OF_2
#define IS_POWER_OF_2(n) ((n) > 0 && (((n) & ((n) - 1)) == 0))
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x, y) (((x) + (y) - 1) / (y))
#endif

#ifndef ALIGN_UP
#define ALIGN_UP(x, a) ((((x) - 1) | ((a) - 1)) + 1)
#endif

#endif /* _COMMONLIB_HELPERS_H_ */
