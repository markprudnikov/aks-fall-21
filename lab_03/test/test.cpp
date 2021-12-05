#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "elf_parser.hpp"



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
