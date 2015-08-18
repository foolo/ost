CXXFLAGS :=	-g -Werror -Wall -fmessage-length=0
CXXFLAGS += -I..

OBJS := \
	page_allocator_test.o \
	tests_main.o \
	page_allocator.o \

LIBS =

TARGET =	unittests.out

default: $(TARGET)

page_allocator.o : ../paging/page_allocator.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
