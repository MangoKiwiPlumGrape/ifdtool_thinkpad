/* Vendored stub — see fix_build.sh for explanation */
#ifndef _FMAP_H_
#define _FMAP_H_
#include <stdint.h>
#define FMAP_STRLEN 32
struct fmap_area { uint32_t offset; uint32_t size; char name[FMAP_STRLEN]; uint16_t flags; } __attribute__((packed));
struct fmap { char signature[8]; uint8_t ver_major; uint8_t ver_minor; uint64_t base; uint32_t size; char name[FMAP_STRLEN]; uint16_t nareas; struct fmap_area areas[]; } __attribute__((packed));
static inline long int fmap_find(const uint8_t *i __attribute__((unused)), int s __attribute__((unused))) { return -1; }
static inline const struct fmap_area *fmap_find_area(const struct fmap *f __attribute__((unused)), const char *n __attribute__((unused))) { return NULL; }
#endif
