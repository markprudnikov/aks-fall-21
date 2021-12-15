#include <iostream>

#include "disassemble.hpp"

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

RV32_Types get_type(uint32_t cmd) {
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

void rv32_parsers::parse_R_type(uint32_t cmd, std::ostream& file, int line) {
    char buff[100];

    char rd = static_cast<char>((cmd >> 7) & 0x1F);
    char rs1 = static_cast<char>((cmd >> 15) & 0x1F);
    char rs2 = static_cast<char>((cmd >> 20) & 0x1F);
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

void rv32_parsers::parse_U_type(uint32_t cmd, std::ostream& file, int line) {
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

void rv32_parsers::parse_S_type(uint32_t cmd, std::ofstream& file, int line) {
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

void rv32_parsers::parse_J_type(uint32_t cmd, std::ofstream& file, int line) {
    char buff[100];
    char rd = static_cast<char>((cmd >> 7) & 0x1F);

    uint8_t first = (cmd >> 31) & 0x1;
    uint16_t second = (cmd >> 12) & 0xFF;
    uint8_t third = (cmd >> 20) & 0x1;
    uint16_t fourth = (cmd >> 21) & 0x3FF;

    auto imm = static_cast<uint16_t> (((((((first << 8) | (second)) << 1) | third) << 10) | fourth) << 1);

    if (imm)
        sprintf(buff, "%08x %10s: %s x%d, %d\n", line, "", "jal", rd, imm);
    else
        sprintf(buff, "%08x %10s: %s x%d, %x\n", line, "", "jal", rd, line);

    file << buff;
}

void rv32_parsers::parse_I_type(uint32_t cmd, std::ofstream& file, int line) {
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

void rv32_parsers::parse_IL_type(uint32_t cmd, std::ofstream& file, int line) {
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

void rv32_parsers::parse_B_type(uint32_t cmd, std::ostream& file, int line) {
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

RVC_Types get_type(uint16_t cmd) {

    auto funct3 = cmd >> 13;
    auto op = cmd & 0x3;

    if (!op && (funct3 == 0))
        return CIW;

    if (!op && !(cmd >> 15))
        return CL;

    if (!op)
        return CS;

    if (op == 1 && ( funct3 == 1 || funct3 == 5 ))
        return CJ;

    if (op == 1 && ( funct3 == 6 || funct3 == 7 ))
        return CB;

    if (op == 1 && funct3 < 4)
        return CI;

    auto imm3 = (cmd >> 10) & 0x7;

    if (op == 1 && funct3 == 4 && (imm3 == 3 || imm3 == 7))
        return CR;

    if (op == 1 && funct3 == 4)
        return CI;

    if (op == 2 && funct3 < 4)
        return CI;

    if (op == 2 && funct3 > 4)
        return CR;

    if (op == 2 && funct3 == 4)
        return CSS;

    return UNKWN;
}

void rvc_parsers::parse_CIW_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    std::string name = "C.ADDI4SPN";
    uint8_t rd = (cmd >> 2) & 0x7;
    uint8_t rs1 = 2;

    uint8_t tail = (cmd >> 6) & 0x1;
    uint8_t pred_tail = (cmd >> 5) & 0x1;
    uint8_t pred_head = (cmd >> 11) & 0x3;
    uint8_t head = (cmd >> 7) & 0xF;
    uint8_t imm = (((((head << 2) | pred_head) << 1) | pred_tail) << 1) | tail;

    sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rs1, imm);

    file << buff;
}

void rvc_parsers::parse_CL_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CS_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CI_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CJ_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CB_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CR_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}

void rvc_parsers::parse_CSS_type(uint16_t cmd, std::ofstream& file, int line) {
    //TODO
}
