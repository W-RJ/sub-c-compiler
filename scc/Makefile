CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

CG = true

# src & build dirs
src = src
build = build

precmd = $(build)/sc.lang

# *.o
objects = $(build)/main.o $(build)/lexer.o $(build)/parser.o $(build)/config.o $(build)/exception.o
ifeq ($(CG),true)
    objects += $(build)/cg.o
endif
# scc[.exe]
ifeq ($(OS),Windows_NT)
    target = $(build)/scc.exe
else
    target = $(build)/scc
endif

# compile *.cpp -> *.o
compile = $(CXX) $(CXXFLAGS) -c $(src)/$(1).cpp -o $(build)/$(1).o

## targets

# make scc[.exe]
$(target): $(objects) Makefile $(precmd)
	$(CXX) $(CXXFLAGS) $(objects) -o $(target)

# make *.o
$(build)/main.o: $(src)/main.cpp $(src)/lexer.h $(src)/parser.h $(src)/config.h \
        $(src)/exception.h $(src)/sc.lang Makefile $(precmd)
	$(call compile,main)

$(build)/lexer.o: $(src)/lexer.cpp $(src)/lexer.h $(src)/regexp $(src)/regexp.h $(src)/regexp.tcc $(src)/trie $(src)/trie.h \
        $(src)/trie.tcc $(src)/config.h $(src)/exception.h $(src)/sc.lang Makefile $(precmd)
	$(call compile,lexer)

$(build)/parser.o: $(src)/parser.cpp $(src)/parser.h Makefile $(precmd)
	$(call compile,parser)

$(build)/config.o: $(src)/config.cpp $(src)/config.h Makefile $(precmd)
	$(call compile,config)

$(build)/exception.o: $(src)/exception.cpp $(src)/exception.h Makefile $(precmd)
	$(call compile,exception)

$(build)/cg.o: $(src)/cg.cpp $(src)/cg.h Makefile $(precmd)
	$(call compile,cg)

# mkdir & sc.lang
$(precmd): $(src)/sc.lang Makefile
	mkdir -p build
	cp $(src)/sc.lang $(build)/sc.lang

## phony targets

.PHONY : test all clean

test: $(target)
	# TODO

# clean & rebuild
all: clean $(target)

clean:
	-rm $(objects) $(target) $(build)/sc.lang
