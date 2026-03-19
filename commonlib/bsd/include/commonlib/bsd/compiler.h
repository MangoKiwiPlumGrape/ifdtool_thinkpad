/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Vendored stub for commonlib/bsd/compiler.h
 * Provides __packed and other compiler attributes required by ifdtool.
 */

#ifndef _COMMONLIB_BSD_COMPILER_H_
#define _COMMONLIB_BSD_COMPILER_H_

#ifndef __packed
#define __packed	__attribute__((packed))
#endif

#ifndef __aligned
#define __aligned(x)	__attribute__((aligned(x)))
#endif

#ifndef __always_inline
#define __always_inline	inline __attribute__((always_inline))
#endif

#ifndef __noreturn
#define __noreturn	__attribute__((noreturn))
#endif

#ifndef __unused
#define __unused	__attribute__((unused))
#endif

#endif /* _COMMONLIB_BSD_COMPILER_H_ */
