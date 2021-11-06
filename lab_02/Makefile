CFLAGS=-c -Wall -Wextra -Werror -I include/ -std=c++14
CXX=g++

all: lab_02

lab_02: obj/main.o obj/cache.o
	$(CXX) obj/main.o obj/cache.o -o lab_02

obj:
	mkdir obj

obj/cache.o: src/cache.cpp include/cache.hpp | obj
	$(CXX) $(CFLAGS) src/cache.cpp -o obj/cache.o

obj/main.o: src/main.cpp include/my_array.hpp include/cache.hpp | obj
	$(CXX) $(CFLAGS) src/main.cpp -o obj/main.o

.PHONY: clean
clean:
	rm -rf obj/ lab_02