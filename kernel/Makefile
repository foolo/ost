# todo arch by parameter
include ../toolchain/toochain_env.sh
export PATH := $(PATH):$(ELFCC)/bin:$(OSTCC)/bin
export CPLUS_INCLUDE_PATH := /usr/local/ost/include/c++/4.9.2
HOSTARCH=ia32
CC=i686-elf-gcc
AR=i686-elf-ar
BUILDDIR=build-$(HOSTARCH)

LIBDIR := $(OSTROOT)/usr/lib

INCFLAGS :=
INCFLAGS += -I$(OSTROOT)/usr/include
INCFLAGS += -I$(OSTROOT)/usr/include/sys
INCFLAGS += -I../toolchain  #for newlib/i686-ost/syscalls.h
INCFLAGS += -Iinclude
INCFLAGS += -I.

COMMONFLAGS := $(INCFLAGS) -ffreestanding -O0 -g -Wall -Wextra -Werror=return-type

CPPFLAGS := $(COMMONFLAGS) -fno-exceptions -fno-rtti

CFLAGS := $(COMMONFLAGS) -std=gnu99

ASMFLAGS := $(COMMONFLAGS)

LDFLAGS := -ffreestanding -O2 -nostdlib

LDPATH :=
LDPATH += -L$(LIBDIR)
LDPATH += -L../libk/build-ia32

LIBS= -lgcc -lc -lg -lm -lk

LIBK_FILE=$(LIBDIR)/libg.a

BINOUT=$(BUILDDIR)/myos.bin
ISOOUT=$(BUILDDIR)/myos.iso

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
	syscalls/syscalls_bypass.cpp \
	memory/page_allocator.cpp \
	memory/multiboot_mmap.cpp \
	memory/MemoryRange.cpp \
	memory/ia32/gdt.cpp \
	memory/ia32/paging.cpp \
	process/elf_loader.cpp \
	storage/ata/ia32/ide_controller.cpp \
	util/md5/md5.c \
	util/md5/md5digest.cpp \

CFILES=

ASMFILES=\
	boot.S \
	interrupts/ia32/interrupts.S \
	memory/ia32/gdt.S \
	memory/ia32/paging.S \

CPP_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(CPPFILES))
C_OBJFILES :=   $(patsubst %, $(BUILDDIR)/%.o, $(CFILES))
ASM_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(ASMFILES))

default: $(ISOOUT)

-include $(CPP_OBJFILES:.o=.d)
-include $(C_OBJFILES:.o=.d)

# Dependencies
$(BUILDDIR)/boot.S.o: ../init/init.bin

# Compile
$(BUILDDIR)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(CC) -c $*.S -o $(BUILDDIR)/$*.S.o $(ASMFLAGS)

$(BUILDDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.cpp.o $(CPPFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.cpp.o $(CPPFLAGS) $*.cpp > $(BUILDDIR)/$*.cpp.d

$(BUILDDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c  $*.c -o $(BUILDDIR)/$*.c.o $(CFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.c.o $(CFLAGS) $*.c > $(BUILDDIR)/$*.c.d


# Link
 

CRTI_OBJ=$(BUILDDIR)/cpp/i686-elf/crti.S.o
CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=  $(shell $(CC) -print-file-name=crtend.o)
CRTN_OBJ=$(BUILDDIR)/cpp/i686-elf/crtn.S.o

#Ordering is important here. ASM_OBJFILES needs to be first so that multiboot_header is in the first page.
OBJS:=$(ASM_OBJFILES) $(CPP_OBJFILES) $(C_OBJFILES)

OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ)   $(OBJS)   $(CRTEND_OBJ) $(CRTN_OBJ)


LIBCDEPS := $(wildcard $(OSTROOT)/usr/lib/*)
$(BINOUT): $(OBJ_LINK_LIST) $(LIBK_FILE) linker.ld $(LIBCDEPS)
	@mkdir -p $(dir $@)
	# LIBS occurs twice in the linker command because LIBC depends on on syscalls_bypass.cpp.o
	# An alternative would have been to place syscall_bypass.o at the end of the command
	$(CC) -T linker.ld -o $(BINOUT) $(LIBS) $(OBJ_LINK_LIST) $(LDFLAGS) $(LDPATH) $(LIBS)


# Create .iso

$(ISOOUT): $(BINOUT)
	@echo Creating $@
	mkdir -p $(BUILDDIR)/isodir/boot/grub
	cp $(BINOUT) $(BUILDDIR)/isodir/boot/
	cp grub.cfg   $(BUILDDIR)/isodir/boot/grub/
	grub-mkrescue -o $@ $(BUILDDIR)/isodir

.PHONY: clean all

clean:
	rm -rf $(BUILDDIR)
