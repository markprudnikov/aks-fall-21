#include "writer.hpp"
#include "symtab.hpp"

#include <iostream>

unsigned short shift(const Parcel parcel) {
    if ((parcel & 0x3F) == 0x1F)
        return 2;
    if ((parcel & 0x7F) == 0x3F)
        return 3;
    std::cerr << "Command length is more than 64 bit\n";
    exit(1);
}

bool is32BitCmd(const Parcel parcel) {
    return (parcel & 0x3) == 0x3 && ((parcel >> 2) & 0x7) != 0x7;
}

bool is16BitCmd(const Parcel parcel) {
    return (parcel & 0x3) != 0x3;
}

// IL -- Type I, Load instructions
enum Types {
    R, I, S, B, U, J, IL
};

void parse_R_type(uint32_t cmd, std::ostream& file, int line) {
    char buff[100];

    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    char rs1 = static_cast<char>((cmd >> 15) & 0x1F);
    char rs2 = static_cast<char>((cmd >> 20) & 0x1F);;
    std::string name;
    switch ((cmd >> 12) & 0x7) {
        case 0:
            if ((cmd >> 25) == 1)
                name = "mul";
            else if (cmd >> 25)
                name = "sub";
            else
                name = "add";
            break;
        case 1:
            if (cmd >> 25)
                name = "mulh";
            else
                name = "sll";
                    break;
        case 2:
            if (cmd >> 25)
                name = "mulhsu";
            else
                name = "slt";
                    break;
        case 3:
            if (cmd >> 25)
                name = "mulhu";
            else
                name = "sltu";
            break;
        case 4:
            if (cmd >> 25)
                name = "div";
            else
                name = "xor";
            break;
        case 5:
            if ((cmd >> 25) == 1)
                name = "divu";
            else if (cmd >> 25)
                name = "sra";
            else
                name = "srl";
            break;
        case 6:
            if (cmd >> 25)
                name = "rem";
            else
                name = "or";
            break;
        case 7:
            if (cmd >> 25)
                name = "remu";
            else
                name = "and";
            break;
        default:
            name = "unknown";
    }

    sprintf(buff, "%08x %10s: %s x%d, x%d, x%d\n", line, "", name.c_str(), rd, rs1, rs2);

    file << buff;
}

void parse_U_type(uint32_t cmd, std::ostream& file, int line) {
    char buff[100];

    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    uint32_t imm = (cmd >> 12) & 0xFFFFF;
    std::string name;
    if ((cmd & 0x3F) == 0b0110111)
        name = "lui";
    else if ((cmd & 0x3F) == 0b0010111)
        name = "auipc";
    else
        name = "unknown_u";


    sprintf(buff, "%08x %10s: %s x%d, %x\n", line, "", name.c_str(), rd, imm);

    file << buff;
}

void parse_S_type(uint32_t cmd, std::ofstream& file, int line) {
    char buff[100];

    uint8_t tail = (cmd >> 7) & 0x1F;
    uint8_t head = (cmd >> 25) & 0x7F;
    char imm = static_cast<char>((head << 5) | tail);
    uint8_t rs1 = (cmd >> 15) & 0x1F;
    uint8_t rs2 = (cmd >> 20) & 0x1F;
    std::string name;

    switch ((cmd >> 12) & 0x7) {
        case 0:
            name = "sb";
            break;
        case 1:
            name = "sh";
            break;
        case 2:
            name = "sw";
            break;
    }

    sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", name.c_str(), rs2, imm, rs1);

    file << buff;
}

void parse_J_type(uint32_t cmd, std::ofstream& file, int line) {
    char buff[100];
    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    std::string name;

    file << "J type\n";
}

void parse_I_type(uint32_t cmd, std::ofstream& file, int line) {
    char buff[100];
    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    char rs1 = static_cast<char>((cmd >> 15) & 0x1F);
    char imm = static_cast<char>((cmd >> 20) & 0xFFF);

    // check jalr
    if ((cmd & 0x7F) == 0b1100111) {
        sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", "jalr", rd, rs1, imm);
        file << buff;
        return;
    }

    std::string name;
    switch ((cmd >> 12) & 0x7) {
        case 0:
            name = "addi";
            break;
        case 1:
            name = "slli";
            break;
        case 2:
            name = "slti";
            break;
        case 3:
            name = "sltiu";
            break;
        case 4:
            name = "xori";
            break;
        case 5:
            if (cmd >> 20)
                name = "srai";
            else
                name = "srli";
            break;
        case 6:
            name = "ori";
            break;
        case 7:
            name = "andi";
            break;
        default:
            name = "unknown_i";
    }

    sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rs1, imm);
    file << buff;
}

void parse_IL_type(uint32_t cmd, std::ofstream& file, int line) {
    char buff[100];
    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    char rs1 = static_cast<char>((cmd >> 15) & 0x1F);
    char imm = static_cast<char>((cmd >> 20) & 0xFFF);
    std::string name;

    switch ((cmd >> 12) & 0x7) {
        case 0:
            name = "lb";
            break;
        case 1:
            name = "lh";
            break;
        case 2:
            name = "lw";
            break;
        case 4:
            name = "lbu";
            break;
        case 6:
            name = "lhu";
            break;
        default:
            name = "unknown_il";
    }

    sprintf(buff, "%08x %10s: %s x%d, %d(%d)\n", line, "", name.c_str(), rd, imm, rs1);
    file << buff;
}

void parse_B_type(uint32_t cmd, std::ostream& file, int line) {
    char buff[100];

    uint8_t rs1 = (cmd >> 15) & 0x1F;
    uint8_t rs2 = (cmd >> 20) & 0x1F;
    std::string name;

    uint8_t first = cmd >> 31;
    uint8_t second = (cmd >> 7) & 0x1;
    uint8_t third  = (cmd >> 25) & 0x3F;
    uint8_t fourth = (cmd >> 8) & 0xF;
    auto imm = static_cast<uint16_t>(((((((first << 1) | second) << 6) | third) << 4) | fourth) << 1);

    switch ((cmd >> 12) & 0x7) {
        case 0:
            name = "beq";
            break;
        case 1:
            name = "bne";
            break;
        case 3:
            name = "blt";
            break;
        case 5:
            name = "bge";
            break;
        case 6:
            name = "bltu";
            break;
        case 7:
            name = "bgeu";
            break;
    }
    if (imm)
        sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rs1, rs2, imm);
    else
        sprintf(buff, "%08x %10s: %s x%d, x%d, %x\n", line, "", name.c_str(), rs1, rs2, line);

    file << buff;
}

Types get_type(uint32_t cmd) {
    switch (cmd & 0x7F) {
        case 0x33:
            return R;
        case 0x63:
            return B;
        case 0x23:
            return S;
        case 0x6F:
            return J;
        case 0x37:
        case 0x17:
            return U;
        case 0x3:
            return IL;
        case 0x67:
        case 0x13:
            return I;

        default:
            std::cerr << "Unknown command\n";
            exit(1);
    }
}

void writeByType(Types type, uint32_t cmd, std::ofstream& file, int line) {
    switch (type) {
        case R:
            parse_R_type(cmd, file, line);
            break;
        case U:
            parse_U_type(cmd, file, line);
            break;
        case S:
            parse_S_type(cmd, file, line);
            break;
        case I:
            parse_I_type(cmd, file, line);
            break;
        case IL:
            parse_IL_type(cmd, file, line);
            break;
        case J:
            parse_J_type(cmd, file, line);
            break;
        case B:
            parse_B_type(cmd, file, line);
            break;
        default:
            file << "unknown type\n";
    }
}

void writeTextSection(std::ofstream& file, TextSection& text_sec) {
    int line = 0;
    for (std::size_t i = 0; i < text_sec.size(); ++i) {
        if (is32BitCmd(text_sec[i])) {
            uint16_t tail = text_sec[i];
            uint16_t head = text_sec[++i];
            uint32_t cmd = (head << 16) | tail;
            Types type = get_type(cmd);
            writeByType(type, cmd, file, line);
            line += 4;
        }
        else if (is16BitCmd(text_sec[i])) {
            uint16_t cmd = text_sec[i];
            line += 2;
        }
        else {
            i += shift(text_sec[i]);
            file << "unknown command\n";
        }
    }

    file << "\n";
}

void writeSymbolTable(std::ofstream& file, SymbolTable& sym_tab, const char* str_tab) {
    writer_impl::writeHeader(file);

    for (std::size_t i = 0; i < sym_tab.size(); ++i)
        writer_impl::writeSymbol(file, sym_tab[i], str_tab, i);
}

void writer_impl::writeHeader(std::ofstream& file) {
    char str[LINE_SZ]{ '\0' };

    sprintf(
        str,
        HEADER_FORMAT,
        "Symbol",
        "Value",
        "Size",
        "Type",
        "Bind",
        "Vis",
        "Index",
        "Name"
    );

    file << str;
}

void writer_impl::writeSymbol(std::ofstream& file, Symbol& symbol, const char* str_tab, std::size_t index) {
    char str[LINE_SZ];

    sprintf(
        str,
        SYMBOL_FORMAT,
        static_cast<int>(index),
        symbol.st_value,
        symbol.st_size,
        SymTab::Getters::get_type(symbol.st_info),
        SymTab::Getters::get_bind(symbol.st_info),
        SymTab::Getters::get_visibility(symbol.st_other),
        SymTab::Getters::get_section_index(symbol.st_shndx),
        str_tab + symbol.st_name
    );

    file << str;
}