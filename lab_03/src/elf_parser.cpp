#include <fstream>
#include <iostream>
#include "elf_parser.hpp"

ElfHeader elf_parsers::extract_elf_header(std::ifstream& file) {
    ElfHeader elf_header;

    file.read(reinterpret_cast<char*>(elf_header.e_ident), sizeof(elf_header));

    return elf_header;
}

SectionHeaderArray elf_parsers::extract_section_header_array(std::ifstream& file, Elf32_Half sh_size,  Elf32_Half sh_quantity) {
    SectionHeaderArray sh_array(sh_quantity);

    for (std::size_t i = 0; i < sh_quantity; ++i) {
        char* p = reinterpret_cast<char *>(&(sh_array[i].sh_name));
        file.read(p, sh_size);
    }

    return sh_array;
}

HeaderStringTable elf_parsers::extract_header_string_table(std::ifstream& file, Elf32_Word size) {
    HeaderStringTable header_str_table(size, '\0');
    char ch;

    for (int i = 0; i < size; i++) {
        file.read(&ch, sizeof(ch));
        header_str_table[i] = ch;
    }

    return header_str_table;
}

Elf32_Word elf_parsers::find_section_index(HeaderStringTable& header_str_table, std::string section_name) {
    Elf32_Word index = 0;

    for (int i = header_str_table.find(section_name); i >= 0; i--)
        if (header_str_table[i] == '\0')
            index += 1;

    return index;
}

void elf_parsers::extract_section_in_file(SectionHeader& section_header, std::ifstream& file, std::ofstream& output) {
    file.seekg(section_header.sh_offset);
    char ch;
    
    for (std::size_t i = 0; i < section_header.sh_size; i++) {    
        file.read(&ch, sizeof(ch));
        output.write(&ch , sizeof(ch));
    }
    
}