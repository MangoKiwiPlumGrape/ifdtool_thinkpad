# SPDX-License-Identifier: BSD-3-Clause

# Object files for ifdtool
ifdtoolobj = ifdtool.o fmap.o kv_pair.o valstr.o

# Relative paths: no hardcoded $(top)
IFDTOOLCFLAGS  = -O2 -g -Wall -Wextra -Wmissing-prototypes -Werror
IFDTOOLCFLAGS += -I ./commonlib/include -I ./commonlib/bsd/include
IFDTOOLCFLAGS += -I ./flashmap
IFDTOOLCFLAGS += -include ./commonlib/bsd/include/commonlib/bsd/compiler.h
IFDTOOLCFLAGS += -D_DEFAULT_SOURCE # for endianness converting functions

# Compile ifdtool sources
$(objutil)/ifdtool/%.o: ./ifdtool/%.c
	$(HOSTCC) $(IFDTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

# Compile flashmap sources
$(objutil)/ifdtool/%.o: ./flashmap/%.c
	$(HOSTCC) $(IFDTOOLCFLAGS) $(HOSTCFLAGS) -c -o $@ $<

# Link ifdtool
$(objutil)/ifdtool/ifdtool: $(addprefix $(objutil)/ifdtool/,$(ifdtoolobj))
	@printf "   IFDTOOL\n"
	$(HOSTCC) ${LDFLAGS} $(addprefix $(objutil)/ifdtool/,$(ifdtoolobj)) -o $@
