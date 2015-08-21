# todo arch by parameter
include ../toolchain/toochain_env.sh
export PATH := $(PATH):$(ELFCC)/bin:$(OSTCC)/bin
HOSTARCH=ia32
CC=i686-elf-gcc
AS=i686-elf-as
AR=i686-elf-ar
BUILDDIR=build-$(HOSTARCH)

LIBDIR=$(OSTROOT)/usr/lib
INCFLAGS := -I$(OSTROOT)/usr/include
INCFLAGS += -I../toolchain  #for newlib/i686-ost/syscalls.h
INCFLAGS += -Iinclude
CXXFLAGS := $(INCFLAGS) -ffreestanding -O2 -g -Wall -Wextra
CPPFLAGS := $(CXXFLAGS) -fno-exceptions -fno-rtti

CFLAGS=$(CXXFLAGS) -std=gnu99

LDFLAGS := -ffreestanding -O2 -nostdlib

LDPATH := -L$(LIBDIR)
LDPATH += -L../libk/build-ia32

LIBS= -lgcc -lc -lg -lm -lk

LIBK_FILE=$(LIBDIR)/libg.a

BINOUT=$(BUILDDIR)/myos.bin

# Source files

CPPFILES=\
	terminal/$(HOSTARCH)/terminal.cpp \
	kernel.cpp \
	cpp/heap.cpp \
	interrupts/$(HOSTARCH)/pic.cpp \
	interrupts/$(HOSTARCH)/idt.cpp \
	keyboard/keyboard.cpp \
	keyboard/ia32/keyboard_controller.cpp \
	syscalls/syscall-handler.cpp \
	paging/page_allocator.cpp \

CFILES=

ASMFILES=\
	boot.S \
	interrupts/ia32/interrupts.s \

CPP_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(CPPFILES))
C_OBJFILES :=   $(patsubst %, $(BUILDDIR)/%.o, $(CFILES))
ASM_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(ASMFILES))

default: $(BINOUT)

-include $(CPP_OBJFILES:.o=.d)
-include $(C_OBJFILES:.o=.d)


# Compile

$(BUILDDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $*.s -o $(BUILDDIR)/$*.s.o

$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(CC) -c $*.S -o $(BUILDDIR)/$*.S.o $(INCFLAGS)

$(BUILDDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.cpp.o $(CPPFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.cpp.o $(CPPFLAGS) $*.cpp > $(BUILDDIR)/$*.cpp.d

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c  $*.c -o $(BUILDDIR)/$*.c.o $(CFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.c.o $(CFLAGS) $*.c > $(BUILDDIR)/$*.c.d


# Link


CRTI_OBJ=$(BUILDDIR)/cpp/i686-elf/crti.s.o
CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=  $(shell $(CC) -print-file-name=crtend.o)
CRTN_OBJ=$(BUILDDIR)/cpp/i686-elf/crtn.s.o

OBJS:=$(CPP_OBJFILES) $(C_OBJFILES) $(ASM_OBJFILES)
OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ)   $(OBJS)   $(CRTEND_OBJ) $(CRTN_OBJ)


LIBCDEPS := $(wildcard $(OSTROOT)/usr/lib/*)
$(BINOUT): $(OBJ_LINK_LIST) $(LIBK_FILE) linker.ld $(LIBCDEPS)
	@mkdir -p $(dir $@)
	$(CC) -T linker.ld -o $(BINOUT) $(OBJ_LINK_LIST) $(LDFLAGS) $(LDPATH) $(LIBS)


# Create .iso

%.iso: $(BINOUT)
	@echo Creating $@
	mkdir -p build/isodir/boot/grub
	cp $(BINOUT) build/isodir/boot/myos.bin
	cp grub.cfg   build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/isodir

.PHONY: clean all

clean:
	rm -rf $(BUILDDIR)
