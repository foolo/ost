# todo arch by parameter
include ../toolchain/toochain_env.sh
export PATH := $(PATH):$(ELFCC)/bin:$(OSTCC)/bin
HOSTARCH=ia32
CC=x86_64-elf-gcc
AS=x86_64-elf-as
AR=x86_64-elf-ar
BUILDDIR=build-$(HOSTARCH)


CPPFLAGS := -O2 -g -Wall -Wextra -ffreestanding -fbuiltin -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
CPPFLAGS += -Iinclude
CPPFLAGS += -I$(OSTROOT)/usr/include


all: $(BUILDDIR)/libk.a

OBJS:=\
	$(BUILDDIR)/libk.cpp.o \

#-include $(OBJS:.o=.d)


.PHONY: all clean

$(BUILDDIR)/libk.a: $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJS)

$(BUILDDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
	$(CC) -MM -MQ $@ $(CFLAGS) $(CPPFLAGS) $< > $(BUILDDIR)/$*.cpp.d

clean:
	rm -rf $(BUILDDIR)
