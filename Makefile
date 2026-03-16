# SPDX-License-Identifier: GPL-2.0-only
# ifdtool-thinkpad — standalone Makefile (GitHub patched for universal build)

PROGRAM = ifdtool
CC      ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

# Use relative paths to commonlib so ZIP or git clone works
CFLAGS  ?= -O2 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
           -I ./commonlib/include -I ./commonlib/bsd/include

# Required object files
OBJS = ifdtool.o fmap.o kv_pair.o valstr.o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PROGRAM) *.o *~ .dependencies

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean install
