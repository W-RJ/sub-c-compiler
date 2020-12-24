CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

# root & src & build dirs
root = ..
src = src
build = build

precmd = $(build)/sc.lang

option = $(build)/option

ifeq ($(OS),Windows_NT)
    CXXFLAGS += -D WINDOWS
    t2str = $(root)/tools/$(build)/t2str.exe
else
    t2str = $(root)/tools/$(build)/t2str
endif

# *.o
objects = $(build)/main.o $(build)/lexer.o $(build)/parser.o $(build)/config.o
externs = $(root)/common/build/exception.o
ifeq ($(CG),4)
    externs += $(root)/sci/build/imain.o $(root)/sci/build/interpreter.o
endif

# scc[.exe]
ifeq ($(OS),Windows_NT)
    target = $(build)/scc.exe
else
    target = $(build)/scc
endif

# options
ifdef CG
    objects += $(build)/cg.o
else
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

# make scc[.exe]
$(target): $(objects) $(externs) Makefile $(precmd)
	$(CXX) $(CXXFLAGS) $(objects) $(externs) -o $(target)

# make *.o
$(build)/main.o: $(src)/main.cpp $(src)/lexer.h $(src)/parser.h $(src)/config.h \
        $(src)/define.h $(root)/common/$(src)/exception.h $(src)/sc.lang Makefile $(precmd)
	$(call compile,main)

$(build)/lexer.o: $(src)/lexer.cpp $(src)/lexer.h $(src)/regexp $(src)/regexp.h \
        $(src)/regexp.tcc $(src)/trie $(src)/trie.h $(src)/trie.tcc $(src)/define.h \
        $(root)/common/$(src)/exception.h $(src)/sc.lang Makefile $(precmd)
	$(call compile,lexer)

$(build)/parser.o: $(src)/parser.cpp $(src)/parser.h $(src)/lexer.h $(src)/trie \
        $(src)/trie.h $(src)/trie.tcc $(src)/define.h $(root)/common/$(src)/exception.h \
        $(root)/common/$(src)/pcode.h $(src)/sc.lang Makefile $(precmd)
	$(call compile,parser)

$(build)/config.o: $(src)/config.cpp $(src)/config.h $(root)/common/$(src)/exception.h $(src)/define.h Makefile $(precmd)
	$(call compile,config)

$(build)/cg.o: $(src)/cg.cpp $(src)/cg.h Makefile $(precmd)
	$(call compile,cg)

ifeq ($(t2str),$(wildcard $(t2str)))
$(src)/cg.cpp: $(t2str) $(src)/sc.lang $(src)/cg.tl Makefile
	cp $(src)/cg.tl $(src)/cg.cpp
	$(t2str) < $(src)/sc.lang >> $(src)/cg.cpp
	echo "\";" >> $(src)/cg.cpp
	echo "}" >> $(src)/cg.cpp
endif

# mkdir & sc.lang
$(precmd): $(src)/sc.lang Makefile
	mkdir -p $(build)
	cp $(src)/sc.lang $(build)/sc.lang

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
ifdef CG
	export SCC=$$PWD/$(target); \
	mkdir -p test/cg$(CG); \
	pytest test/cg$(CG); \
	RET=$$?; \
	if [ "$$RET" != "0" ] && [ "$$RET" != "5" ]; then \
		exit $$RET; \
	else \
		exit 0; \
	fi
	# TODO
else
	export SCC=$$PWD/$(target); \
	mkdir -p test/ncg; \
	pytest test/ncg; \
	RET=$$?; \
	if [ "$$RET" != "0" ] && [ "$$RET" != "5" ]; then \
		exit $$RET; \
	else \
		exit 0; \
	fi
	# TODO
endif

# clean & rebuild
all: clean main

clean:
	-rm $(objects) $(target) $(build)/option $(build)/sc.lang
