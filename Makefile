# SPDX-License-Identifier: GPL-2.0-only
# ifdtool-thinkpad — standalone Makefile (patched by fix_build.sh)
PROGRAM = ifdtool
CC      ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local
CFLAGS  ?= -O2 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
           -I /home/util/Documents/ifdtool_thinkpad-main/commonlib/include -I /home/util/Documents/ifdtool_thinkpad-main
OBJS = ifdtool.o
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
