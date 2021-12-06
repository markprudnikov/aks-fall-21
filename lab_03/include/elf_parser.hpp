#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "elf.hpp"

using SectionHeaderArray = std::vector<SectionHeader>;
using HeaderStringTable = std::string;

namespace elf_parsers {
    
    ElfHeader extract_elf_header(std::ifstream& file);

    SectionHeaderArray extract_section_header_array(std::ifstream& file, Elf32_Off sh_off, Elf32_Half sh_size, Elf32_Half sh_quantity);

    HeaderStringTable extract_header_string_table(std::ifstream& file, Elf32_Off str_table_off, Elf32_Word size);

    Elf32_Word find_section_index(HeaderStringTable& header_str_table, SectionHeaderArray& sh_array, const char* section_name);

    void extract_section_to_file(SectionHeader const& section_header, std::ifstream& file, std::ofstream& output);
    
    std::vector<SymbolTable> extract_symbol_table(SectionHeader const& symtab, std::ifstream& file);
}