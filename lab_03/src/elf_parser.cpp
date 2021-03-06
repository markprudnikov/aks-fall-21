#include <iostream>
#include <cstring>

#include "elf.hpp"
#include "elf_parser.hpp"
#include "writer.hpp"

ElfHeader extractElfHeader(std::ifstream& file) {
    ElfHeader elf_header{};

    char* p = reinterpret_cast<char*>(&elf_header);
    file.read(p, sizeof(elf_header));

    return elf_header;
}

SectionHeaderArray extractSectionHeaderArray(std::ifstream& file, const ElfHeader& elf_header) {
    Elf32_Half sh_num = elf_header.e_shnum;
    Elf32_Half sh_size = elf_header.e_shentsize;
    SectionHeaderArray sh_array(sh_num);

    elf_parsers::set_offset(file, elf_header.e_shoff);

    for (std::size_t i = 0; i < sh_num; ++i) {
        char* p = reinterpret_cast<char*>(&(sh_array[i]));
        file.read(p, sh_size);
    }

    sh_array.shrink_to_fit();
    return sh_array;
}

HeaderStringTable getHeaderStringTable(std::ifstream& file, SectionHeaderArray& sh_array, Elf32_Half hst_index) {
    SectionHeader str_table_header = sh_array[hst_index];
    Elf32_Half size = str_table_header.sh_size;
    HeaderStringTable header_str_table(size, '\0');
    char ch;

    elf_parsers::set_offset(file, str_table_header.sh_offset);

    for (std::size_t i = 0; i < size; i++) {
        file.read(&ch, sizeof(ch));
        header_str_table[i] = ch;
    }
    
    return header_str_table;
}

TextSection extractTextSection(std::ifstream& file, HeaderStringTable const& hdr_str_table, SectionHeaderArray& sh_array) {
    SectionHeader text_sec_header = elf_parsers::getSectionHeader(sh_array, hdr_str_table, TEXT_SECTION);

    Elf32_Word size = text_sec_header.sh_size / 2;
    TextSection text_section(size);
    char* p;

    elf_parsers::set_offset(file, text_sec_header.sh_offset);

    for (std::size_t i = 0; i < size; ++i) {
        p = reinterpret_cast<char*> (&text_section[i]);
        file.read(p, sizeof(text_section[i]));
    }

    return text_section;
}

SymbolTable extractSymbolTable(std::ifstream& file, HeaderStringTable const& hdr_str_table, SectionHeaderArray& sh_array) {
    SectionHeader sym_tab_header = elf_parsers::getSectionHeader(sh_array, hdr_str_table, SYMTAB_SECTION);

    Elf32_Word size = sym_tab_header.sh_size / sym_tab_header.sh_entsize;
    SymbolTable symbol_table(size);
    char* p;

    elf_parsers::set_offset(file, sym_tab_header.sh_offset);

    for (std::size_t i = 0; i < size; ++i) {
        p = reinterpret_cast<char*> (&symbol_table[i]);
        file.read(p, sizeof(symbol_table[i]));
    }

    return symbol_table;
}

SectionHeader elf_parsers::getSectionHeader(SectionHeaderArray& sh_array, HeaderStringTable const& hdr_str_table, const char* section_name) {
    Elf32_Word index = getSectionIndex(hdr_str_table, sh_array, section_name);
    return sh_array[index];
}

Elf32_Word elf_parsers::getSectionIndex(HeaderStringTable const& header_str_table, SectionHeaderArray& sh_array, const char* section_name) {
    Elf32_Word index = 0;

    for (; index < sh_array.size(); ++index) {
        const char* p = header_str_table.c_str();
        p += sh_array[index].sh_name;

        if (!strcmp(p, section_name))
            return index;
    }

    return index;
}

void elf_parsers::set_offset(std::ifstream& file, Elf32_Off offset) {
    file.clear();
    file.seekg(0);
    file.seekg(offset);
}
