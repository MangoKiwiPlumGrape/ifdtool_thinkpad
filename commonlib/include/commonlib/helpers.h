/* SPDX-License-Identifier: BSD-3-Clause */
/* Vendored minimal stub — only macros needed by ifdtool */
#ifndef _COMMONLIB_HELPERS_H_
#define _COMMONLIB_HELPERS_H_
#include <stddef.h>
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif
#ifndef MIN
#define MIN(a, b) ({ __typeof__(a) _a=(a); __typeof__(b) _b=(b); _a<_b?_a:_b; })
#endif
#ifndef MAX
#define MAX(a, b) ({ __typeof__(a) _a=(a); __typeof__(b) _b=(b); _a>_b?_a:_b; })
#endif
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x,y) (((x)+(y)-1)/(y))
#endif
#ifndef ALIGN_UP
#define ALIGN_UP(x,a) ((((x)-1)|((a)-1))+1)
#endif
#ifndef IS_POWER_OF_2
#define IS_POWER_OF_2(x) ((x) != 0 && ((x) & ((x)-1)) == 0)
#endif
#ifndef __packed
#define __packed __attribute__((packed))
#endif
#ifndef __unused
#define __unused __attribute__((unused))
#endif
#endif /* _COMMONLIB_HELPERS_H_ */
