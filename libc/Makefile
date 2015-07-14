# todo arch by parameter
HOSTARCH=ia32
CC=i686-elf-gcc
AS=i686-elf-as
AR=i686-elf-ar


BUILDDIR=build-$(HOSTARCH)

CFLAGS=-O2 -g -Wall -Wextra $(ARCH_CFLAGS)
CPPFLAGS=-Iinclude $(ARCH_CPPFLAGS)
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)


FREE_CPPFILES:=\
$(ARCH_FREE_CPPFILES) \
stdio.cpp \
string.cpp \

HOSTED_CPPFILES:=\
$(ARCH_HOSTED_CPPFILES) \
# place normal hosted objs here

LIBK_OBJS:=$(FREEOBJS:.o=.libk.o)


FREE_CPP_OBJFILES :=   $(patsubst %, $(BUILDDIR)/%.o, $(FREE_CPPFILES))
HOSTED_CPP_OBJFILES := $(patsubst %, $(BUILDDIR)/%.o, $(HOSTED_CPPFILES))

BINARIES=$(BUILDDIR)/libc.a $(BUILDDIR)/libg.a $(BUILDDIR)/libk.a
all: $(BINARIES)

-include $(FREE_CPP_OBJFILES:.o=.d)
-include $(HOSTED_CPP_OBJFILES:.o=.d)

LIBK_OBJS:=$(FREE_CPP_OBJFILES:.o=.libk.o)

OBJS:=\
$(FREE_CPP_OBJFILES) \
$(HOSTED_CPP_OBJFILES) \



.PHONY: all clean

$(BUILDDIR)/libc.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(BUILDDIR)/libg.a:
	$(AR) rcs $@

$(BUILDDIR)/libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

$(BUILDDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
	$(CC) -MM -MQ $@ $(CFLAGS) $(CPPFLAGS) $< > $(BUILDDIR)/$*.cpp.d

$(BUILDDIR)/%.cpp.libk.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)
#	$(CC) -MM -MQ $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS) $< > $(BUILDDIR)/$*.cpp.d

clean:
	rm -rf $(BUILDDIR)
