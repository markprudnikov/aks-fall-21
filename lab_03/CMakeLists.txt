cmake_minimum_required(VERSION 3.17)
project(lab_03)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(
        lab_03
        src/main.cpp src/elf_parser.cpp src/symtab.cpp src/writer.cpp
        include/elf.hpp include/elf_parser.hpp include/writer.hpp include/symtab.hpp
        src/disassemble.cpp include/disassemble.hpp)