# todo arch by parameter
HOSTARCH=ia32
CC=i686-elf-gcc
AS=i686-elf-as
AR=i686-elf-ar
BUILDDIR=build-$(HOSTARCH)


LIBCDIR=../libc

CXXFLAGS=-ffreestanding -O2 -g -Wall -Wextra
CXXFLAGS+=-I$(LIBCDIR)/include
CXXFLAGS+=-Iinclude
CPPFLAGS=$(CXXFLAGS) -fno-exceptions -fno-rtti
CFLAGS=$(CXXFLAGS) -std=gnu99

LDFLAGS=-ffreestanding -O2 -nostdlib -L$(LIBCDIR)/$(BUILDDIR)
LIBS=-lgcc -lk

LIBK_FILE=$(LIBCDIR)/$(BUILDDIR)/libk.a

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

CFILES=

ASMFILES=\
	boot.s \
	interrupts/ia32/interrupts.s \

CPP_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(CPPFILES))
C_OBJFILES :=   $(patsubst %, $(BUILDDIR)/%.o, $(CFILES))
ASM_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(ASMFILES))

# Order is significant. libc makefile shall be run first.
default: libc $(BINOUT)

.PHONY: libc
libc:
	make -C ../libc

-include $(CPP_OBJFILES:.o=.d)
-include $(C_OBJFILES:.o=.d)


# Compile

$(BUILDDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $*.s -o $(BUILDDIR)/$*.s.o

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

$(BINOUT): $(OBJ_LINK_LIST) $(LIBK_FILE)
	@mkdir -p $(dir $@)
	$(CC) -T linker.ld -o $(BINOUT) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)


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
	make -C ../libc clean
