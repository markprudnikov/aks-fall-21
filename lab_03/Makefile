CFLAGS=-c -I include/ -std=c++14
CXX=g++

all: lab_03

lab_03: obj/elf_parser.o obj/main.o
	$(CXX) obj/elf_parser.o obj/main.o -o lab_03

testing: obj/test.o obj/elf_parser.o
	$(CXX) obj/test.o obj/elf_parser.o -o testing

obj/test.o: test/test.cpp include/doctest.h include/elf_parser.hpp | obj
	$(CXX) $(CFLAGS) test/test.cpp -o obj/test.o

obj:
	mkdir obj

obj/elf_parser.o: src/elf_parser.cpp include/elf_parser.hpp | obj
	$(CXX) $(CFLAGS) src/elf_parser.cpp -o obj/elf_parser.o

obj/main.o: src/main.cpp include/elf_parser.hpp | obj
	$(CXX) $(CFLAGS) src/main.cpp -o obj/main.o

.PHONY: clean
clean:
	rm -rf obj/ lab_03 testing
