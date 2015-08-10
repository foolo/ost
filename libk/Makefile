# todo arch by parameter
HOSTARCH=ia32
CC=i686-elf-gcc
AS=i686-elf-as
AR=i686-elf-ar
BUILDDIR=build-$(HOSTARCH)


CPPFLAGS := -O2 -g -Wall -Wextra -ffreestanding -fbuiltin
CPPFLAGS += -Iinclude
CPPFLAGS += -I/usr/local/ost/sysroot/usr/include


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
