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

# pull in dependency info for *existing* .o files
-include $(CPP_OBJFILES:.o=.d)


DIRS=$(BUILDDIR) $(dir $(CPP_OBJFILES))

all: post-build

pre-build:
	@mkdir -p $(DIRS)

post-build: main-build
#	@echo POST

main-build: pre-build
	@$(MAKE) --no-print-directory $(BINNAME)

# Compile

$(BUILDDIR)/boot.o: src/boot.s
	$(AS) src/boot.s -o $(BUILDDIR)/boot.o

$(BUILDDIR)/%.o: %.cpp
	$(CC) -c  $*.cpp -o $(BUILDDIR)/$*.o $(CPPFLAGS)
	$(CC) -MM $(CPPFLAGS) $*.cpp > $*.d


# Link

$(BINNAME): $(CPP_OBJFILES) $(BUILDDIR)/boot.o
	$(CC) -T src/linker.ld -o $(BINNAME) $^ $(LDFLAGS) $(LIBS)

.PHONY: clean all

clean:
	rm -rf $(BUILDDIR) $(BINNAME)

