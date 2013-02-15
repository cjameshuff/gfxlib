
# Makefile for clang/libc++ projects
# For libc++ on Mac OS X 10.6:
# http://thejohnfreeman.com/blog/2012/11/07/building-libcxx-on-mac-osx-10.6.html

#******************************************************************************

LINK=llvm-link
CC=clang
CXX=clang++
AR=llvm-ar
AS=llvm-as
NM=llvm-nm

#******************************************************************************

EXECNAME = simple

INCLUDES += -Isrc
INCLUDES += -I../../src

VPATH = src ../../src

SOURCE = main.cpp
SOURCE += filestore.cpp


# Avoid bunch of errors in math.h: "unknown type name '__extern_always_inline'"
DEFINES += -D__extern_always_inline=inline
INCLUDES += -I/llvm-svn/include/c++/v1
LIBS += -L/llvm-svn/lib
LIBS += -lc++
# LIBS += -lstdc++

# -U__STRICT_ANSI__ required for math.h bug on OS X 10.6
# CFLAGS = -g -O3 -ffast-math -msse4.1
CFLAGS += -g -O3 -ffast-math -msse4.1
CFLAGS += $(DEFINES) $(INCLUDES)

CXXFLAGS += -stdlib=libc++
CXXFLAGS += -std=c++11 $(CFLAGS)

#******************************************************************************
# Generate lists of object and dependency files
#******************************************************************************
CSOURCES = $(filter %.c,$(SOURCE))
CLSOURCES = $(filter %.cl,$(SOURCE))
CPPSOURCES = $(filter %.cpp,$(SOURCE))

BITCODE = $(addprefix bc/, $(CSOURCES:.c=.c.bc)) \
          $(addprefix bc/, $(CLSOURCES:.cl=.cl.bc)) \
          $(addprefix bc/, $(CPPSOURCES:.cpp=.cpp.bc))

#******************************************************************************
# Dependency rules
#******************************************************************************

.PHONY: all default clean depend echo none disasm

default: $(EXECNAME) Makefile

run: $(EXECNAME) Makefile
	./$(EXECNAME)

install:

clean:

clean:
	rm -rf obj
	rm -rf disasm
	rm -rf bc
	rm -f $(EXECNAME)
	rm -rf $(EXECNAME).dSYM


$(EXECNAME): $(BITCODE)
	$(CC) $(CFLAGS) $(BITCODE) $(LIBS) $(LDFLAGS) -o $@

bc/%.c.bc: %.c
	$(shell if [ ! -d $(addprefix bc/, $(dir $<)) ]; then mkdir -p $(addprefix bc/, $(dir $<)); fi )
	$(CC) -emit-llvm $(CFLAGS) -c $< -o $@

bc/%.cl.bc: %.cl
	$(shell if [ ! -d $(addprefix bc/, $(dir $<)) ]; then mkdir -p $(addprefix bc/, $(dir $<)); fi )
	$(CC) -x cl -emit-llvm $(CFLAGS) -c $< -o $@

bc/%.cpp.bc: %.cpp
	$(shell if [ ! -d $(addprefix bc/, $(dir $<)) ]; then mkdir -p $(addprefix bc/, $(dir $<)); fi )
	$(CXX) -emit-llvm $(CXXFLAGS) -c $< -o $@


#******************************************************************************
# End of file
#******************************************************************************