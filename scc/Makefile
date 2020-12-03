CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall

precmd = build/sc.lang
objects = build/main.o build/lexer.o build/parser.o build/config.o build/exception.o

build/scc: $(objects) Makefile $(precmd)
	$(CXX) $(CXXFLAGS) $(objects) -o build/scc

build/main.o: src/main.cpp src/lexer.h src/parser.h src/config.h src/sc.lang Makefile $(precmd)
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o build/main.o

build/lexer.o: src/lexer.cpp src/lexer.h src/sc.lang Makefile $(precmd)
	$(CXX) $(CXXFLAGS) -c src/lexer.cpp -o build/lexer.o

build/parser.o: src/parser.cpp src/parser.h Makefile $(precmd)
	$(CXX) $(CXXFLAGS) -c src/parser.cpp -o build/parser.o

build/config.o: src/config.cpp src/config.h Makefile $(precmd)
	$(CXX) $(CXXFLAGS) -c src/config.cpp -o build/config.o

build/exception.o: src/exception.cpp src/exception.h Makefile $(precmd)
	$(CXX) $(CXXFLAGS) -c src/exception.cpp -o build/exception.o

$(precmd): src/sc.lang Makefile
	mkdir -p build
	cp src/sc.lang build/sc.lang

.PHONY : clean
clean:
	-rm $(objects) build/sc.lang build/scc
