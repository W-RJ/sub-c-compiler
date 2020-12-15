CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

# root & src & build dirs
root = ..
src = src
build = build

# t2wcs[.exe]
ifeq ($(OS),Windows_NT)
    t2wcs = $(build)/t2wcs.exe
    word2input = $(build)/word2input.exe
    CXXFLAGS += -D WINDOWS
else
    t2wcs = $(build)/t2wcs
    word2input = $(build)/word2input
endif

## targets

.PHONY : main test all clean

main: $(t2wcs) $(word2input)

# make t2wcs[.exe]
$(t2wcs): $(src)/t2wcs.cpp Makefile
	mkdir -p $(build)
	$(CXX) $(CXXFLAGS) -D t2wcs=main $(src)/t2wcs.cpp -o $(t2wcs)

# make word2input[.exe]
$(word2input): $(src)/word2input.cpp Makefile
	mkdir -p $(build)
	$(CXX) $(CXXFLAGS) -D word2input=main $(src)/word2input.cpp -o $(word2input)

## phony targets

test: $(t2wcs) $(word2input)
	# TODO

# clean & rebuild
all: clean $(t2wcs) $(word2input)

clean:
	-rm $(t2wcs) $(word2input)
