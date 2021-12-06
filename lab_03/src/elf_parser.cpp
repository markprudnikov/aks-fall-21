#include <fstream>
#include <iostream>
#include <cstring>

#include "elf.hpp"
#include "elf_parser.hpp"

ElfHeader elf_parsers::extract_elf_header(std::ifstream& file) {
    ElfHeader elf_header;

    file.read(reinterpret_cast<char*>(elf_header.e_ident), sizeof(elf_header));

    return elf_header;
}

SectionHeaderArray elf_parsers::extract_section_header_array(std::ifstream& file, Elf32_Off sh_off, Elf32_Half sh_size,  Elf32_Half sh_quantity) {
    file.seekg(sh_off);
    SectionHeaderArray sh_array(sh_quantity);

    for (std::size_t i = 0; i < sh_quantity; ++i) {
        char* p = reinterpret_cast<char *>(&(sh_array[i]));
        file.read(p, sh_size);
    }

    return sh_array;
}

HeaderStringTable elf_parsers::extract_header_string_table(std::ifstream& file, Elf32_Off str_table_off, Elf32_Word size) {
    file.seekg(str_table_off);
    HeaderStringTable header_str_table(size, '\0');
    char ch;

    for (int i = 0; i < size; i++) {
        file.read(&ch, sizeof(ch));
        header_str_table[i] = ch;
    }

    return header_str_table;
}

Elf32_Word elf_parsers::find_section_index(HeaderStringTable& header_str_table, SectionHeaderArray& sh_array, const char* section_name) {
    Elf32_Word index = 0;

    for ( ; index < sh_array.size(); ++index) {
        const char * p = header_str_table.c_str();    
        p += sh_array[index].sh_name; 

        if (!strcmp(p, section_name))
            return index;
    }
    
    return index; 
}

void elf_parsers::extract_section_to_file(SectionHeader const& section_header, std::ifstream& file, std::ofstream& output) {
    file.clear();
    file.seekg(0);
    file.seekg(section_header.sh_offset);
    char ch;
    
    for (std::size_t i = 0; i < section_header.sh_size; i++) {    
        file.read(&ch, sizeof(ch));
        output.write(&ch , sizeof(ch));
    }
    
}