CC=i686-elf-gcc
AS=i686-elf-as

CXXFLAGS=-ffreestanding -O2 -Wall -Wextra
CXXFLAGS+=-Isrc/libc/include
CXXFLAGS+=-Isrc/kernel/include
CPPFLAGS=$(CXXFLAGS) -fno-exceptions -fno-rtti
CFLAGS=$(CXXFLAGS) -std=gnu99

LDFLAGS=-ffreestanding -O2 -nostdlib
LIBS=-lgcc

BINNAME=myos.bin

BUILDDIR=build_i386


# Source files

CPPFILES=\
	src/kernel/terminal/i386/Terminal.cpp \
	src/kernel/kernel.cpp \
	src/kernel/cpp/heap.cpp \
	src/libc/stdio.cpp

CFILES=\
	src/libc/string/string.c

ASMFILES=src/boot.s src/interrupt.s

CPP_OBJFILES := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CPPFILES))
C_OBJFILES :=   $(patsubst %.c,   $(BUILDDIR)/%.o, $(CFILES))
ASM_OBJFILES := $(patsubst %.s,   $(BUILDDIR)/%.o, $(ASMFILES))

default: $(BINNAME)

-include $(CPP_OBJFILES:.o=.d)
-include $(C_OBJFILES:.o=.d)


# Compile

$(BUILDDIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $*.s -o $(BUILDDIR)/$*.o

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.o $(CPPFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.o $(CPPFLAGS) $*.cpp > $(BUILDDIR)/$*.d

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c  $*.c -o $(BUILDDIR)/$*.o $(CFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.o $(CFLAGS) $*.c > $(BUILDDIR)/$*.d


# Link


CRTI_OBJ=$(BUILDDIR)/src/kernel/cpp/i686-elf/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=  $(shell $(CC) -print-file-name=crtend.o)
CRTN_OBJ=$(BUILDDIR)/src/kernel/cpp/i686-elf/crtn.o

OBJS:=$(CPP_OBJFILES) $(C_OBJFILES) $(ASM_OBJFILES)
OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ)   $(OBJS)   $(CRTEND_OBJ) $(CRTN_OBJ)

$(BINNAME): $(OBJ_LINK_LIST)
	$(CC) -T src/linker.ld -o $(BINNAME) $(OBJ_LINK_LIST) $(LDFLAGS) $(LIBS)


# Create .iso

%.iso: $(BINNAME)
	@echo Creating $@
	mkdir -p build/isodir/boot/grub
	cp $(BINNAME) build/isodir/boot/myos.bin
	cp grub.cfg   build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/isodir

.PHONY: clean all

clean:
	rm -rf $(BUILDDIR) $(BINNAME)
