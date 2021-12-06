#include <iostream>
#include <fstream>
#include <cstring>

#include "elf.hpp"
#include "elf_parser.hpp"

#define TEXT_SECTION ".text"
#define SYMTAB_SECTION ".symtab"

// objdump -s -j .text test_elf.o
// readelf -x .text test_elf.o

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

std::string get_type(unsigned char info) {
    unsigned char res = info & 0xF;
    if (res == 0)
        return "NOTYPE";
    if (res == 1)
        return "OBJECT";
    if (res == 2)
        return "FUNC";
    if (res == 3)
        return "SECTION";
    if (res == 4)
        return "FUNCTION";
    
    return "SMTH";
}

std::string get_bind(unsigned char info) {
    unsigned char res = (info >> 4);    
    if (res == 0)
        return "LOCAL";
    if (res == 1)
        return "GLOBAL";
    if (res == 2)
        return "WEAK";
    if (res == 10)
        return "LOOS";
    if (res == 12)
        return "HIOS";
    if (res == 13)
        return "LOPROC";
    if (res == 15)
        return "HIPROC";
}

int main(int argc, char** argv) {
    
    std::ifstream file(argv[1]);

    ElfHeader elf_header = elf_parsers::extract_elf_header(file);
    Elf32_Half str_table_index = elf_header.e_shstrndx;
    Elf32_Off sh_offset = elf_header.e_shoff;
    Elf32_Half sh_size = elf_header.e_shentsize;
    Elf32_Half sh_quantity = elf_header.e_shnum;

    SectionHeaderArray sh_array = elf_parsers::extract_section_header_array(file, sh_offset, sh_size, sh_quantity);
    Elf32_Off str_table_off = sh_array[str_table_index].sh_offset;
    Elf32_Word str_table_size = sh_array[str_table_index].sh_size;

    HeaderStringTable header_str_table = elf_parsers::extract_header_string_table(file, str_table_off, str_table_size);
    
    Elf32_Word text_index = elf_parsers::find_section_index(header_str_table, sh_array, TEXT_SECTION);
    Elf32_Word symtab_index = elf_parsers::find_section_index(header_str_table, sh_array, SYMTAB_SECTION);
    SectionHeader text = sh_array[text_index];
    SectionHeader symtab = sh_array[symtab_index];
    
    std::ofstream text_output("text_output");
    elf_parsers::extract_section_to_file(text, file, text_output);

    std::ofstream output("sym_table.txt");
    std::vector<SymbolTable> st_array = elf_parsers::extract_symbol_table(symtab, file);

    char p1[100];
    sprintf(p1, "%s %-15s %7s %-8s %-8s %-8s %6s %s\n", "Symbol", "Value", "Size", "Type", "Bind", "Vis", "Index", "Name");
    //output.write(p1, 100);
    output << p1;
    char const * ptr = header_str_table.c_str();

    char p[100];
    for (int i = 0; i < st_array.size(); ++i) {
        SymbolTable s = st_array[i];
        sprintf(p,
        "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n",
        i, s.st_value, s.st_size, get_type(s.st_info).c_str(), get_bind(s.st_info).c_str(), "DEFAULT", "2", ptr + s.st_name);
        //output.write(p, 100);
        output << p;
    }

    file.close();
    text_output.close();
    output.close();
    return 0;
}