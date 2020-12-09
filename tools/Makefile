CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

# root & src & build dirs
root = ..
src = src
build = build

# t2wcs[.exe]
ifeq ($(OS),Windows_NT)
    target = $(build)/t2wcs.exe
else
    target = $(build)/t2wcs
endif

## targets

# make t2wcs[.exe]
$(target): $(src)/t2wcs.cpp Makefile
	mkdir -p $(build)
	$(CXX) $(CXXFLAGS) -D t2wcs=main $(src)/t2wcs.cpp -o $(target)

## phony targets

.PHONY : test all clean

test: $(target)
	# TODO

# clean & rebuild
all: clean $(target)

clean:
	-rm $(target)
