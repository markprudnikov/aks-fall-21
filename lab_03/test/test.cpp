#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "elf.hpp"
#include "elf_parser.hpp"
#include <iostream>

TEST_CASE("Extract header from 32-bit elf") {
    std::ifstream file("hello_i386_32");

    ElfHeader elf_header = elf_parsers::extract_elf_header(file);
    CHECK(elf_header.e_ident[0] == 127);
    CHECK(elf_header.e_ident[1] == 'E');
    CHECK(elf_header.e_ident[2] == 'L');
    CHECK(elf_header.e_ident[3] == 'F');
    CHECK(elf_header.e_ident[4] == 1); // 32 or 64 bit
    CHECK(elf_header.e_type == 2); // EXEC or smth
    CHECK(elf_header.e_machine == 3); // intel 
    CHECK(elf_header.e_version == 1);
    CHECK(elf_header.e_ehsize == 52);
    CHECK(elf_header.e_entry == 0x08048000);

    CHECK(elf_header.e_phentsize == 32);
    CHECK(elf_header.e_phnum == 2);

    CHECK(elf_header.e_shentsize == 40);
    CHECK(elf_header.e_shnum == 4);

    CHECK(elf_header.e_shstrndx == 1);

    file.close();
}

//TODO("Test extracting section headers")

TEST_CASE("Extract header string table") {
    std::ifstream file("hello_i386_32");

    ElfHeader elf_header = elf_parsers::extract_elf_header(file);
    SectionHeaderArray sh_array = elf_parsers::extract_section_header_array(file, elf_header.e_shoff, elf_header.e_shentsize, elf_header.e_shnum);
    
    CHECK(sh_array[0].sh_type == 0);
    CHECK(sh_array[1].sh_type == 3);
    CHECK(sh_array[2].sh_type == 1);
    CHECK(sh_array[3].sh_type == 1);

    Elf32_Off str_table_off = sh_array[elf_header.e_shstrndx].sh_offset;
    CHECK(str_table_off == 0x00102e);
    Elf32_Word str_table_size = sh_array[elf_header.e_shstrndx].sh_size;
    CHECK(str_table_size == 0x000017);

    HeaderStringTable table = elf_parsers::extract_header_string_table(file, str_table_off, str_table_size);
    std::string answer;
    answer += '\0';
    answer += ".shstrtab";
    answer += '\0';
    answer += ".text";
    answer += '\0';
    answer += ".data";
    answer += '\0';
    CHECK(answer.size() == table.size());
    for (int i = 0; i < answer.size(); i++) {
        std::cout << answer[i];
    }
    std::cout << std::endl;
    for (int i = 0; i < table.size(); i++) {
        std::cout << table[i];
    }
    std::cout << std::endl;
    CHECK(!table.compare(answer));


    file.close();
}

TEST_CASE("Get symbol table") {
    std::ifstream file("test_elf.o");   

    ElfHeader elf_header = elf_parsers::extract_elf_header(file);
    Elf32_Half str_table_index = elf_header.e_shstrndx;
    Elf32_Off sh_offset = elf_header.e_shoff;
    Elf32_Half sh_size = elf_header.e_shentsize;
    Elf32_Half sh_quantity = elf_header.e_shnum;

    SectionHeaderArray sh_array = elf_parsers::extract_section_header_array(file, sh_offset, sh_size, sh_quantity);
    Elf32_Off str_table_off = sh_array[str_table_index].sh_offset;
    Elf32_Word str_table_size = sh_array[str_table_index].sh_size;

    HeaderStringTable header_str_table = elf_parsers::extract_header_string_table(file, str_table_off, str_table_size);
    
    Elf32_Word symtab_index = elf_parsers::find_section_index(header_str_table, sh_array, ".symtab");

    SectionHeader symtab = sh_array[symtab_index];
    
    std::vector<SymbolTable> st_array = elf_parsers::extract_symbol_table(symtab, file);

    SUBCASE("Check Sizes") {
        CHECK(st_array[0].st_size == 0);
        CHECK(st_array[1].st_size == 0);
        CHECK(st_array[2].st_size == 0);
        CHECK(st_array[3].st_size == 0);
        CHECK(st_array[4].st_size == 0);
        CHECK(st_array[5].st_size == 0);
        CHECK(st_array[6].st_size == 124);
    }

    SUBCASE("Check Values") {
        CHECK(st_array[0].st_value == 0);
        CHECK(st_array[1].st_value == 0);
        CHECK(st_array[2].st_value == 0x00000030);
        CHECK(st_array[3].st_value == 0x00000040);
        CHECK(st_array[4].st_value == 0x00000058);
        CHECK(st_array[5].st_value == 0x00000068);
        CHECK(st_array[6].st_value == 0);
    }

    SUBCASE("Check Types") {
        CHECK(st_array[0].st_info == 0);
        CHECK(st_array[1].st_info == 4); // (0 << 4) + 4 & (0xF)
        CHECK(st_array[2].st_info == 0);
        CHECK(st_array[3].st_info == 0);
        CHECK(st_array[4].st_info == 0);
        CHECK(st_array[5].st_info == 0);
        CHECK(st_array[6].st_info == 18); // (1 << 4) + 2 & (0xF)
    }
    
    SUBCASE("Check Visibility") {
        CHECK(st_array[0].st_other == 0);
        CHECK(st_array[1].st_other == 0);
        CHECK(st_array[2].st_other == 0);
        CHECK(st_array[3].st_other == 0);
        CHECK(st_array[4].st_other == 0);
        CHECK(st_array[5].st_other == 0);
        CHECK(st_array[6].st_other == 0);
    }

    SUBCASE("Check Ndx") {
        CHECK(st_array[0].st_shndx == 0);
        CHECK(st_array[1].st_shndx == 0xfff1);
        CHECK(st_array[2].st_shndx == 2);
        CHECK(st_array[3].st_shndx == 2);
        CHECK(st_array[4].st_shndx == 2);
        CHECK(st_array[5].st_shndx == 2);
        CHECK(st_array[6].st_shndx == 2);
    }

    SUBCASE("Check names") {
        const char* p = header_str_table.c_str();
        CHECK(*(p + st_array[0].st_name) == '\0');
        CHECK(strcmp(p + st_array[1].st_name, "test2.c") == 0);
        CHECK(strcmp(p + st_array[2].st_name, ".LBB0_1") == 0);
        CHECK(strcmp(p + st_array[3].st_name, ".LBB0_2") == 0);
        CHECK(strcmp(p + st_array[4].st_name, ".LBB0_3") == 0);
        CHECK(strcmp(p + st_array[5].st_name, ".LBB0_4") == 0);
        CHECK(strcmp(p + st_array[6].st_name, "main") == 0);
    }
    
    file.close();
}