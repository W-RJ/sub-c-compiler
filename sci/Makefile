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
objects = $(build)/imain.o $(build)/interpreter.o

# scc[.exe]
ifeq ($(OS),Windows_NT)
    target = $(build)/sci.exe
else
    target = $(build)/sci
endif

ifdef release
    CXXFLAGS += -D NDEBUG
else
    CXXFLAGS += -D DEBUG
endif

# compile *.cpp -> *.o
compile = $(CXX) $(CXXFLAGS) -c $(src)/$(1).cpp -o $(build)/$(1).o

## targets

.PHONY : main options test unit_test module_test all clean

main: $(precmd) options $(target)

# make scc[.exe]
$(target): $(objects) Makefile $(precmd)
	$(CXX) $(CXXFLAGS) $(objects) -o $(target)

# make *.o
$(build)/imain.o: $(src)/imain.cpp $(src)/interpreter.h Makefile $(precmd)
	$(call compile,imain) -D imain=main

$(build)/interpreter.o: $(src)/interpreter.cpp $(src)/interpreter.h Makefile $(precmd)
	$(call compile,interpreter)

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

test: unit_test module_test

unit_test:
	# TODO

module_test: main
	export SCC=$$PWD/$(target); \
	pytest test/ncg
	# TODO

# clean & rebuild
all: clean main

clean:
	-rm $(objects) $(target) $(build)/option $(build)/sc.lang
