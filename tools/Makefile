CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -Wno-unused-result

# root & src & build dirs
root = ..
src = src
build = build

# t2str[.exe]
ifeq ($(OS),Windows_NT)
    t2str = $(build)/t2str.exe
    word2input = $(build)/word2input.exe
    CXXFLAGS += -D WINDOWS
else
    t2str = $(build)/t2str
    word2input = $(build)/word2input
endif

## targets

.PHONY : main test all clean

main: $(t2str) $(word2input)

# make t2str[.exe]
$(t2str): $(src)/t2str.cpp Makefile
	mkdir -p $(build)
	$(CXX) $(CXXFLAGS) -D t2str=main $(src)/t2str.cpp -o $(t2str)

# make word2input[.exe]
$(word2input): $(src)/word2input.cpp Makefile
	mkdir -p $(build)
	$(CXX) $(CXXFLAGS) -D word2input=main $(src)/word2input.cpp -o $(word2input)

## phony targets

test: main
	# TODO

# clean & rebuild
all: clean $(t2str) $(word2input)

clean:
	-rm $(t2str) $(word2input)
