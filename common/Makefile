CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

# root & src & build dirs
root = ..
src = src
build = build

precmd = $(build)/.dir

option = $(build)/option

ifeq ($(OS),Windows_NT)
    CXXFLAGS += -D WINDOWS
endif

# *.o
objects = $(build)/exception.o

ifdef release
    CXXFLAGS += -D NDEBUG
else
    CXXFLAGS += -D DEBUG
endif

# compile *.cpp -> *.o
compile = $(CXX) $(CXXFLAGS) -c $(src)/$(1).cpp -o $(build)/$(1).o

## targets

.PHONY : main options test unit_test module_test all clean

main: $(precmd) options $(objects)

# make *.o
$(build)/exception.o: $(src)/exception.cpp $(src)/exception.h Makefile $(precmd)
	$(call compile,exception)

# mkdir & sc.lang
$(precmd): Makefile
	mkdir -p $(build)
	touch $(build)/.dir

# process options
options:
ifneq ($(option),$(wildcard $(option)))
	touch $(src)/define.h
	echo "$(CG)\n$(release)" > $(option)
else
	{ read prerelease; } < $(option); \
	if [ "x$(release)" != "x$$prerelease" ]; then \
		touch $(src)/define.h; \
		echo "$(release)" > $(option); \
	fi
endif

## phony targets

test: unit_test

unit_test:
	# TODO

# clean & rebuild
all: clean main

clean:
	-rm $(objects) $(target) $(build)/option
