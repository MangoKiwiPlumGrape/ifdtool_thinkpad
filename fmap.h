/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Vendored stub for fmap.h — ifdtool standalone build.
 * --validate and --fmap-layout are disabled; all other operations work.
 */
#ifndef _FMAP_H_
#define _FMAP_H_
#include <stdint.h>
#define FMAP_SIGNATURE "__FMAP__"
#define FMAP_NAMELEN 32
struct fmap_area {
	uint32_t offset;
	uint32_t size;
	uint8_t  name[FMAP_NAMELEN];
	uint16_t flags;
} __attribute__((packed));
struct fmap {
	uint8_t  signature[8];
	uint8_t  ver_major;
	uint8_t  ver_minor;
	uint64_t base;
	uint32_t size;
	uint8_t  name[FMAP_NAMELEN];
	uint16_t nareas;
	struct fmap_area areas[];
} __attribute__((packed));
/* Stub implementations — disables --validate and --fmap-layout only.
 * All HAP bit operations (-d, -p, --altmedisable, -x, -i) work normally. */
static inline long fmap_find(const uint8_t *image, unsigned int len)
{
	(void)image; (void)len;
	return -1; /* not found — --validate disabled in standalone build */
}

static inline struct fmap_area *fmap_find_area(struct fmap *fmap,
					       const char *name)
{
	(void)fmap; (void)name;
	return NULL;
}

#endif /* _FMAP_H_ */
