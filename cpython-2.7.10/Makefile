# todo arch by parameter
include ../toolchain/toochain_env.sh
export PATH := $(PATH):$(ELFCC)/bin:$(OSTCC)/bin
HOSTARCH=ia32
CC=i686-elf-gcc
AS=i686-elf-as
AR=i686-elf-ar
BUILDDIR=build-$(HOSTARCH)

CXXFLAGS=-ffreestanding -g -Wfatal-errors
CXXFLAGS+=-IInclude
CXXFLAGS+=-IModules/expat
CXXFLAGS+=-IModules/zlib
CXXFLAGS+=-I.
CXXFLAGS+=-I$(OSTROOT)/usr/include
CXXFLAGS+=-DPy_NO_ENABLE_SHARED

CFLAGS=$(CXXFLAGS) -std=gnu99

LIBOUT=$(BUILDDIR)/libcpython.a

# Source files

CFILES:= $(shell find . -name '*.c')

OBJS := $(patsubst %, $(BUILDDIR)/%.o, $(CFILES))

default: $(LIBOUT)

-include $(OBJS:.o=.d)


# Compile

$(BUILDDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $*.s -o $(BUILDDIR)/$*.s.o

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c  $*.c -o $(BUILDDIR)/$*.c.o $(CFLAGS)
	$(CC) -MM -MQ  $@ $(CFLAGS) $<  >  $(BUILDDIR)/$*.c.d


# Create lib file

$(LIBOUT): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJS)

.PHONY: clean all

clean:
	rm -rf $(BUILDDIR)
