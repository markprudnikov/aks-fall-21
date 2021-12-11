#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "elf.hpp"

#define TEXT_SECTION ".text"
#define SYMTAB_SECTION ".symtab"


using SectionHeaderArray = std::vector<SectionHeader>;
using HeaderStringTable = std::string;
using SymbolTable = std::vector<Symbol>;
using Parcel = uint16_t;
using TextSection = std::vector<Parcel>;

void disassemble(std::ifstream& src, std::ofstream& dst, ElfHeader const& elf_header);

ElfHeader extractElfHeader(std::ifstream& file);

SectionHeaderArray extractSectionHeaderArray(std::ifstream& file, const ElfHeader& elf_header);

HeaderStringTable getHeaderStringTable(std::ifstream& file, SectionHeaderArray& sh_array, Elf32_Half hst_index);

TextSection extractTextSection(std::ifstream& file, HeaderStringTable const& hdr_str_table, SectionHeaderArray& sh_array);

SymbolTable extractSymbolTable(std::ifstream& file, HeaderStringTable const& hdr_str_table, SectionHeaderArray& sh_array);

namespace elf_parsers {
    
    SectionHeader getSectionHeader(SectionHeaderArray& sh_array, HeaderStringTable const& hdr_str_table, const char* section_name);

    Elf32_Word getSectionIndex(HeaderStringTable const& header_str_table, SectionHeaderArray& sh_array, const char* section_name);

    void set_offset(std::ifstream& file, Elf32_Off offset);

    // void extract_section_to_file(SectionHeader const& section_header, std::ifstream& file, std::ofstream& output);
}