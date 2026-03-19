# SPDX-License-Identifier: GPL-2.0-only
#
# ifdtool-thinkpad — standalone Makefile
#
# No coreboot tree required. Vendored commonlib headers are included
# in the commonlib/ subdirectory of this repo.
#
# Usage:
#   make          — build ifdtool
#   make install  — install to /usr/local/bin
#   make clean    — remove build artifacts

PROGRAM = ifdtool

CC      ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

# Use vendored commonlib headers from within this repo
COMMONLIB_INC     := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))commonlib/include
COMMONLIB_BSD_INC := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))commonlib/bsd/include
REPOROOT          := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

CFLAGS  ?= -O2 -Wall -Wextra \
           -Wno-unused-parameter \
           -Wno-sign-compare \
           -I $(COMMONLIB_INC) \
           -I $(COMMONLIB_BSD_INC) \
           -I $(REPOROOT)

OBJS = ifdtool.o

OS_ARCH = $(shell uname)
ifeq ($(OS_ARCH), Darwin)
# no extra flags needed
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS += -I/usr/local/include
endif

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PROGRAM) *.o *~ .dependencies

distclean: clean

dep:
	@$(CC) $(CFLAGS) -MM *.c > .dependencies 2>/dev/null || true

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean distclean dep install

-include .dependencies
