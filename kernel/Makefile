CC=i686-elf-gcc
AS=i686-elf-as

LIBCDIR=../libc

CXXFLAGS=-ffreestanding -O2 -Wall -Wextra
CXXFLAGS+=-I$(LIBCDIR)/include
CXXFLAGS+=-Iinclude
CPPFLAGS=$(CXXFLAGS) -fno-exceptions -fno-rtti
CFLAGS=$(CXXFLAGS) -std=gnu99

LDFLAGS=-ffreestanding -O2 -nostdlib -L$(LIBCDIR)/build-ia32
LIBS=-lgcc -lk


BUILDDIR=build_i386/kernel

BINOUT=$(BUILDDIR)/myos.bin

# Source files

CPPFILES=\
	terminal/ia32/terminal.cpp \
	kernel.cpp \
	cpp/heap.cpp \

CFILES=

ASMFILES=boot.s

CPP_OBJFILES := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CPPFILES))
C_OBJFILES :=   $(patsubst %.c,   $(BUILDDIR)/%.o, $(CFILES))
ASM_OBJFILES := $(patsubst %.s,   $(BUILDDIR)/%.o, $(ASMFILES))

default: $(BINOUT)

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


CRTI_OBJ=$(BUILDDIR)/cpp/i686-elf/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=  $(shell $(CC) -print-file-name=crtend.o)
CRTN_OBJ=$(BUILDDIR)/cpp/i686-elf/crtn.o

OBJS:=$(CPP_OBJFILES) $(C_OBJFILES) $(ASM_OBJFILES)
OBJ_LINK_LIST:=$(CRTI_OBJ) $(CRTBEGIN_OBJ)   $(OBJS)   $(CRTEND_OBJ) $(CRTN_OBJ)

$(BINOUT): $(OBJ_LINK_LIST)
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
