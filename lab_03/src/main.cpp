#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <vector>

#include "elf_parser.hpp"

// objdump -s -j .text test_elf.o

/**
 * Constraints for the project.
 *
 * e_ident[0] == 127 (Const)
 * e_ident[1] == 'E'
 * e_ident[2] == 'L'
 * e_ident[3] == 'F'
 * e_ident[4] == 1 (32 bit ELF file)
 * e_ident[5] == 1 (LSB little endian)
 * e_machine == 0xF3 (RISC-V)
*/

int main(int argc, char** argv) {
    
    std::ifstream file(argv[1]);

    ElfHeader elf_header = elf_parsers::extract_elf_header(file);
    Elf32_Half str_table_index = elf_header.e_shstrndx;
    Elf32_Off sh_off = elf_header.e_shoff;
    Elf32_Half sh_size = elf_header.e_shentsize;
    Elf32_Half sh_quantity = elf_header.e_shnum;
    SectionHeaderArray sh_array = elf_parsers::extract_section_header_array(file, sh_off, sh_size, sh_quantity);
    Elf32_Off str_table_off = sh_array[str_table_index].sh_offset;
    Elf32_Word str_table_size = sh_array[str_table_index].sh_size;

    file.clear();
    file.seekg(0);
    file.seekg(str_table_off);

    HeaderStringTable header_str_table = elf_parsers::extract_header_string_table(file, str_table_size);
    
    Elf32_Word text_index = 0;
    Elf32_Word symtab_index = 0;

    for (int i = 0; i < sh_array.size(); i++) {
        const char * p = header_str_table.c_str();    
        p += sh_array[i].sh_name; 

        if (!strcmp(p, ".text"))
            text_index = i;
        
        if (!strcmp(p, ".symtab"))
            symtab_index = i;
    }

    SectionHeader text = sh_array[text_index];
    SectionHeader symtab = sh_array[symtab_index];

    std::ofstream text_output("text_output");
    std::ofstream symtab_output("symtab_output");

    elf_parsers::extract_section_to_file(text, file, text_output);
    elf_parsers::extract_section_to_file(symtab, file, symtab_output);

    file.close();
    text_output.close();
    symtab_output.close();

    return 0;
}