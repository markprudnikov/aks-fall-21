CFLAGS=-c -I include/ -std=c++14 -Wall -Wextra -Werror
CXX=g++

all: lab_03

lab_03: obj/elf_parser.o obj/main.o obj/writer.o obj/symtab.o
	$(CXX) obj/elf_parser.o obj/main.o obj/writer.o obj/symtab.o -o lab_03

test: obj/elf_parser.o obj/test.o obj/symtab.o obj/writer.o
	$(CXX) obj/elf_parser.o obj/test.o obj/writer.o obj/symtab.o -o test

obj/test.o: unit_tests/test.cpp include/doctest.h include/elf.hpp include/elf_parser.hpp | obj
	$(CXX) $(CFLAGS) unit_tests/test.cpp -o obj/test.o

obj/writer.o: src/writer.cpp include/writer.hpp include/symtab.hpp | obj
	$(CXX) $(CFLAGS) src/writer.cpp -o obj/writer.o

obj:
	mkdir obj

obj/elf_parser.o: src/elf_parser.cpp include/elf_parser.hpp include/elf.hpp | obj
	$(CXX) $(CFLAGS) src/elf_parser.cpp -o obj/elf_parser.o

obj/main.o: src/main.cpp include/elf_parser.hpp include/elf.hpp include/writer.hpp include/symtab.hpp | obj
	$(CXX) $(CFLAGS) src/main.cpp -o obj/main.o

obj/symtab.o: include/symtab.hpp src/symtab.cpp | obj
	$(CXX) $(CFLAGS) src/symtab.cpp -o obj/symtab.o

.PHONY: clean
clean:
	rm -rf obj/ lab_03 test
