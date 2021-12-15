#pragma once

#include <fstream>
#include <vector>
#include <cstdio>

#include "elf.hpp"
#include "elf_parser.hpp"
#include "disassemble.hpp"

#define LINE_SZ 256
#define SYMBOL_FORMAT "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n"
#define HEADER_FORMAT "%s %-15s %7s %-8s %-8s %-8s %6s %s\n"

void writeTextSection(std::ofstream& file, TextSection& text_sec, SymbolTable& symtab, const char* str_tab, int text_index);
void writeSymbolTable(std::ofstream& file, SymbolTable& sym_tab, const char* str_tab);
void writeByType(RV32_Types type, uint32_t cmd, std::ofstream& file, int line, const char* mark);
void writeRVCbyTypes(RVC_Types type, uint16_t cmd, std::ofstream& file, int line, const char* mark);

namespace writer_impl {
    std::string get_mark(int line, SymbolTable& symtab, const char* str_tab, int text_index);
    void writeHeader(std::ofstream& file);
    void writeSymbol(std::ofstream& file, Symbol& s, const char* str_tab, std::size_t index);
}
