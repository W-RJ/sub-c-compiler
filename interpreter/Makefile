# Module Makefile of SCC.
# Copyright (C) 2020-2021 Renjian Wang
#
# This file is part of SCC.
#
# SCC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SCC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SCC.  If not, see <https://www.gnu.org/licenses/>.

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
externs = $(root)/common/build/exception.o

# scc[.exe]
ifeq ($(OS),Windows_NT)
    target = $(build)/sci.exe
else
    target = $(build)/sci
endif

ifneq ($(CG),4)
    marco = -D imain=main
    override CG=
    CXXFLAGS += -D NCG
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

# make sci[.exe]
$(target): $(objects) $(externs) Makefile $(precmd)
ifneq ($(CG),4)
	$(CXX) $(CXXFLAGS) $(objects) $(externs) -o $(target)
endif

# make *.o
$(build)/imain.o: $(src)/imain.cpp $(src)/interpreter.h $(src)/define.h \
        $(root)/common/src/exception.h Makefile $(precmd)
	$(call compile,imain) $(marco)

$(build)/interpreter.o: $(src)/interpreter.cpp $(src)/interpreter.h \
        $(root)/common/src/pcode.h $(root)/common/src/exception.h Makefile $(precmd)
	$(call compile,interpreter)

# mkdir & sc.lang
$(precmd): Makefile
	mkdir -p $(build)
	touch $(build)/.dir

# process options
options:
	grep "#define CG $(CG)" < $(src)/define.h > /dev/null; \
	if [ $$? != 0 ] ; then \
		sed 's/#define CG [1-5]/#define CG $(CG)/' < $(src)/define.h > $(src)/define_n.h; \
		mv $(src)/define_n.h $(src)/define.h; \
	fi
ifneq ($(option),$(wildcard $(option)))
	touch $(src)/define.h
	echo "$(CG)\n$(release)" > $(option)
else
	{ read precg; read prerelease; } < $(option); \
	if [ "x$(CG)" != "x$$precg" ] || [ "x$(release)" != "x$$prerelease" ]; then \
		touch $(src)/define.h; \
		echo "$(CG)\n$(release)" > $(option); \
	fi
endif

## phony targets

test: unit_test module_test

unit_test:
	# TODO

module_test: main
	# export SCC=$$PWD/$(target); \
	# pytest test/ncg
	# TODO

# clean & rebuild
all: clean main

clean:
	-rm $(objects) $(target) $(build)/option
