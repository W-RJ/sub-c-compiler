CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall

objects = build/main.o build/lexer.o build/parser.o build/config.o

build/scc: $(objects)
	$(CXX) $(CXXFLAGS) $(objects)

build/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o build/main.o

build/lexer.o: src/lexer.cpp src/lexer.h
	$(CXX) $(CXXFLAGS) -c src/lexer.cpp -o build/lexer.o

build/parser.o: src/parser.cpp src/parser.h
	$(CXX) $(CXXFLAGS) -c src/parser.cpp -o build/parser.o

build/config.o: src/config.cpp src/config.h
	$(CXX) $(CXXFLAGS) -c src/config.cpp -o build/config.o

clean:
	-rm $(objects)
