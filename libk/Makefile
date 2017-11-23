# todo arch by parameter
include ../toolchain/toochain_env.sh
export PATH := $(PATH):$(ELFCC)/bin:$(OSTCC)/bin
HOSTARCH=ia32
CC=i686-ost-gcc
AS=i686-ost-as
AR=i686-ost-ar
BUILDDIR=build-$(HOSTARCH)


CPPFLAGS := -O2 -g -Wall -Wextra -ffreestanding -fbuiltin
CPPFLAGS += -Iinclude
CPPFLAGS += -I$(OSTROOT)/usr/include


all: $(BUILDDIR)/libk.a

OBJS:=\
	$(BUILDDIR)/libk.c.o \

#-include $(OBJS:.o=.d)


.PHONY: all clean

$(BUILDDIR)/libk.a: $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJS)

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
	$(CC) -MM -MQ $@ $(CFLAGS) $(CPPFLAGS) $< > $(BUILDDIR)/$*.cpp.d

clean:
	rm -rf $(BUILDDIR)
