#include <iostream>
#include <map>

#include "elf.hpp"
#include "elf_parser.hpp"

// objdump -s -j .text test_elf.o
// readelf -x .text test_elf.o

void check_arguments(int argc, char** argv, std::ifstream& input, std::ofstream& output) {
    if (argc < 3) {
        std::cerr << "Too few arguments" << std::endl;
        exit(1);
    } else if (argc > 3) {
        std::cerr << "Too many arguments" << std::endl;
        exit(1);
    }

    input.open(argv[1]);
    output.open(argv[2]);

    if (!input || !output) {
        std::cerr << "Can't open/create input/output file" << std::endl;
        exit(1);
    }
}

void check_input_file(ElfHeader const& elf_header) {
    char check_sum = 0;

    if (elf_header.e_ident[0] == 0xF)
        check_sum++;
    else if (elf_header.e_ident[1] == 'E')
        check_sum++;
    else if (elf_header.e_ident[2] == 'L')
        check_sum++;
    else if (elf_header.e_ident[3] == 'F')
        check_sum++;
    else if (elf_header.e_ident[4] == 1)
        check_sum++;
    else if (elf_header.e_ident[5] == 1)
        check_sum++;
    else if (elf_header.e_machine == 0xF3)
        check_sum++;

    if (!check_sum) {
        std::cerr << "Not supported format of the input file" << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    std::ifstream input;
    std::ofstream output;

    check_arguments(argc, argv, input, output);
    
    const ElfHeader elf_header = extractElfHeader(input);
    
    check_input_file(elf_header);
    
    disassemble(input, output, elf_header);

    input.close();
    output.close();
    
    return 0;
}