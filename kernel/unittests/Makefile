CXXFLAGS :=	-g -Werror -Wall -fmessage-length=0
CXXFLAGS += -I..
CXXFLAGS += -I../include

OBJS := \
	page_allocator_test.o \
	tests_main.o \
	page_allocator.o \
	MemoryRange.o \

LIBS =

TARGET =	unittests.out

default: $(TARGET)

page_allocator.o : ../paging/page_allocator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

MemoryRange.o : ../memory/MemoryRange.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
