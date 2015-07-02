CC=i686-elf-g++
AS=i686-elf-as

CPPFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

LDFLAGS=-ffreestanding -O2 -nostdlib
LIBS=-lgcc

BINNAME=myos.bin

BUILDDIR=build


# Source files

CPPFILES=\
	src/kernel/terminal/i386/Terminal.cpp \
	src/kernel/kernel.cpp

ASMFILES=src/boot.s src/interrupt.s

CPP_OBJFILES := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CPPFILES))
ASM_OBJFILES := $(patsubst %.s,   $(BUILDDIR)/%.o, $(ASMFILES))

default: $(BINNAME)

-include $(CPP_OBJFILES:.o=.d)


# Compile

$(BUILDDIR)/%.o: %.s
	mkdir -p $(BUILDDIR)
	$(AS) $*.s -o $(BUILDDIR)/$*.o

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $(CPP_OBJFILES))
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.o $(CPPFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.o $(CPPFLAGS) $*.cpp > $(BUILDDIR)/$*.d


# Link

$(BINNAME): $(CPP_OBJFILES) $(ASM_OBJFILES)
	$(CC) -T src/linker.ld -o $(BINNAME) $^ $(LDFLAGS) $(LIBS)


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
