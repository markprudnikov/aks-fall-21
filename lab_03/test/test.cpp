#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
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
    file.seekg(str_table_off);

    HeaderStringTable table = elf_parsers::extract_header_string_table(file, str_table_size);
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