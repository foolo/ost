CC=i686-elf-g++
AS=i686-elf-as

CPPFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti

LDFLAGS=-ffreestanding -O2 -nostdlib
LIBS=-lgcc

BINNAME=myos.bin

BUILDDIR=build


# Source files

CPPFILES=src/kernel.cpp

CPP_OBJFILES := $(patsubst %.cpp, $(BUILDDIR)/%.o, $(CPPFILES))


default: $(BINNAME)

-include $(CPP_OBJFILES:.o=.d)


# Compile

$(BUILDDIR)/boot.o: src/boot.s
	mkdir -p $(BUILDDIR)
	$(AS) src/boot.s -o $(BUILDDIR)/boot.o

$(BUILDDIR)/%.o: %.cpp $(DIRS)
	@mkdir -p $(dir $(CPP_OBJFILES))
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.o $(CPPFLAGS)
	$(CC) -MM -MQ $(BUILDDIR)/$*.o $(CPPFLAGS) $*.cpp > $(BUILDDIR)/$*.d


# Link

$(BINNAME): $(CPP_OBJFILES) $(BUILDDIR)/boot.o
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
