#pragma once

#include "elf.hpp"
#include <fstream>
#include "elf_parser.hpp"
#include <vector>
#include <cstdio>

#define LINE_SZ 256
#define SYMBOL_FORMAT "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n"
#define HEADER_FORMAT "%s %-15s %7s %-8s %-8s %-8s %6s %s\n"

void writeTextSection(std::ofstream& file, TextSection& text_sec, const char* str_tab);
void writeSymbolTable(std::ofstream& file, SymbolTable& sym_tab, const char* str_tab);

namespace writer_impl {
    void writeHeader(std::ofstream& file);
    void writeSymbol(std::ofstream& file, Symbol& s, const char* str_tab, int index);
}
