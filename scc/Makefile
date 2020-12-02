build/scc: build/main.o build/lexer.o build/parser.o
	g++ -o build/lexer.o build/parser.o build/main.o -o build/scc

build/main.o: src/main.cpp
	g++ -c src/main.cpp -o build/main.o

build/lexer.o: src/lexer.cpp
	g++ -c src/lexer.cpp -o build/lexer.o

build/parser.o: src/parser.cpp
	g++ -c src/parser.cpp -o build/parser.o

clean:
	rm build/*
