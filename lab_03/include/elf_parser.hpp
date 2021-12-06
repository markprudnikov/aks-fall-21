#pragma once
#include <cstdint>
#include <string>
#include <vector>

#define EI_NIDENT 16

/* 32-bit ELF base types. */
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;

#pragma pack(push, 1)

struct ElfHeader {
    unsigned char e_ident[EI_NIDENT]; // 16
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;  /* Entry point */
    Elf32_Off	    e_phoff;
    Elf32_Off	    e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
};

struct SectionHeader {
    Elf32_Word	sh_name;
    Elf32_Word	sh_type;
    Elf32_Word	sh_flags;
    Elf32_Addr	sh_addr;
    Elf32_Off	sh_offset;
    Elf32_Word	sh_size;
    Elf32_Word	sh_link;
    Elf32_Word	sh_info;
    Elf32_Word	sh_addralign;
    Elf32_Word	sh_entsize;
};

#pragma pack(pop)

using SectionHeaderArray = std::vector<SectionHeader>;
using HeaderStringTable = std::string;
namespace elf_parsers {

    ElfHeader extract_elf_header(std::ifstream& file);

    SectionHeaderArray extract_section_header_array(std::ifstream& file, Elf32_Off sh_off, Elf32_Half sh_size, Elf32_Half sh_quantity);

    HeaderStringTable extract_header_string_table(std::ifstream& file, Elf32_Word size);

    Elf32_Word find_section_index(HeaderStringTable& header_str_table, std::string section_name);

    void extract_section_to_file(SectionHeader const& section_header, std::ifstream& file, std::ofstream& output);
}