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
    uint8_t rs1 = 2; // always 2

    uint8_t tail = (cmd >> 6) & 0x1;
    uint8_t pred_tail = (cmd >> 5) & 0x1;
    uint8_t pred_head = (cmd >> 11) & 0x3;
    uint8_t head = (cmd >> 7) & 0xF;
    uint8_t imm = (((((head << 2) | pred_head) << 1) | pred_tail) << 1) | tail;
    imm <<= 2; // zero-extended offset
    sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rs1, imm);

    file << buff;
}

void rvc_parsers::parse_CL_CS_types(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    std::string name;
    uint8_t rd = (cmd >> 2) & 0x7;
    uint8_t rs1 = (cmd >> 7) & 0x7;
    auto funct3 = cmd >> 13;
    uint8_t offset = 0;

    if (funct3 == 1) {
        name = "C.FLD";
    } else if (funct3 == 2) {
        name = "C.LW";
    } else if (funct3 == 3) {
        name = "C.FLW";
    } else if (funct3 == 5) {
        name = "C.FSD";
    } else if (funct3 == 6) {
        name = "C.SW";
    } else if (funct3 == 7) {
        name = "C.FSW";
    } else
        name = "unknown command type CL / CS";

    if (funct3 == 1 || funct3 == 5) {
        uint8_t head = (cmd >> 5) & 0x3;
        uint8_t tail = (cmd >> 10) & 0x7;
        offset = (head << 3) | tail;
    } else {
        uint8_t tail = (cmd >> 6) & 0x1;
        uint8_t head = (cmd >> 5) & 0x1;
        uint8_t mid = (cmd >> 10) & 0x7;
        offset = (((head << 3) | mid ) << 1) | tail;
    }

    offset <<= 2; //zero-extended offset

    sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", name.c_str(), rd, offset, rs1);

    file << buff;
}

void rvc_parsers::parse_CI_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    // NOP
    if (cmd == 1) {
        sprintf(buff, "%08x %10s: %s\n", line, "", "C.NOP");
        file << buff;
        return;
    }

    std::string name;
    auto funct3 = cmd >> 13;
    
    if (funct3 == 0) { // SLLI, ADDI
        auto rd = (cmd >> 7) & 0x1F;
        uint8_t head = (cmd >> 12) & 0x1;
        int8_t imm;
        if (head == 0) {
            name = "C.SLLI";
            imm = (cmd >> 2) & 0x1F;
        } else {
            name = "C.ADDI";
            uint8_t tail = (cmd >> 2) & 0x1F;
            imm = (head << 5) | tail;
        }
        sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rd, imm);
        
    } else if (funct3 == 1) { // FLDSP
        uint8_t frd = (cmd >> 7) & 0x1F;
        uint8_t head = (cmd >> 2) & 0x7;
        uint8_t mid = (cmd >> 12) & 0x1;
        uint8_t tail = (cmd >> 5) & 0x3;
        uint16_t offset = (((head << 1) | mid) << 2) | tail;        
        offset <<= 3; // zero extended
        sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", "C.FLDSP", frd, offset, 2);    
    } else if (funct3 == 2) { // LWSP or LI
        uint8_t op = cmd & 0x3;
        uint8_t rd = (cmd >> 7) & 0x1F;
        if (rd == 0) {
            file << "unknown cmd\n";
            return;
        }

        if (op == 1) { // LI
            int8_t simm = ((((cmd >> 12) & 0x1) << 4) | (cmd >> 2) & 0x1F);
            sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", "C.LI", rd, rd, simm);
        } else if (op == 2) { // LWSP
            uint16_t offset;
            uint8_t head = (cmd >> 2) & 0x3;
            uint8_t tail = (cmd >> 4) & 0x7;
            uint8_t mid = (cmd >> 12) & 0x1;
            offset = (((head << 1) | mid) << 3 ) | tail;
            offset <<= 2; // zero extended
            sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", "C.LWSP", rd, offset, 2);
        }
    } else if (funct3 == 3) { //FLWSP or ADDI16SP, LUI
        uint8_t op = cmd & 0x3;
        if (op == 1) { // LUI, ADDI16SP
            uint8_t rd = (cmd >> 7) & 0x1F;
            if (rd == 2) {
                uint8_t head = (cmd >> 12) & 0x1;
                uint8_t pred_head = (cmd >> 3) & 0x3;
                uint8_t tail = (((((((cmd >> 4) & 0x1) << 1) | ((cmd >> 2) & 0x1)) << 1) | ((cmd >> 6) & 0x1)));
                int16_t imm = (((head << 2) | pred_head) << 3) | tail; // 6 bit
                imm <<= 7; // zero extended
                sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", "C.ADDI16SP", rd, rd, imm);
            } else {
                uint8_t head = (cmd >> 12) & 0x1;
                uint8_t tail = (cmd >> 2) & 0x1F;
                int16_t imm = (head << 16) | tail; // zero extended // сколько бит? ответ = 18
                sprintf(buff, "%08x %10s: %s x%d, %d\n", line, "", "C.LUI", rd, imm);
            }


        } else if (op == 2) { // FLWSP
            uint8_t frd = (cmd >> 7) & 0x1F;
            uint8_t head = (cmd >> 2) & 0x3;
            uint8_t tail = (cmd >> 4) & 0x7;
            uint8_t mid = (cmd >> 12) & 0x1;
            uint16_t offset = (((head << 1) | mid) << 3 ) | tail;
            offset <<= 2; // zero extended
            sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", "C.FLWSP", frd, offset, 2);
        } else {
            sprintf(buff, "unknown command\n");
        }
    } else if (funct3 == 4) { // SRLI, SRAI, ANDI
        uint8_t rd = (cmd >> 7) & 0x7;
        if ((cmd >> 10) & 0x3 != 2) {
            name = "C.ANDI";
            int8_t uimm = (cmd >> 2) & 0x1F;
            sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rd, uimm);    
        } else if ((cmd >> 10) & 0x3 == 0) {
            name = "C.SRLI";
            uint8_t simm = (((cmd >> 12) & 0x1) << 5) | (cmd >> 2) & 0x1F;
            sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rd, simm);    
        } else if ((cmd >> 10) & 0x3 == 0) {
            name = "C.SRAI";
            uint8_t simm = (((cmd >> 12) & 0x1) << 5) | (cmd >> 2) & 0x1F;
            sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rd, rd, simm);    
        }
    }


    file << buff;
}

void rvc_parsers::parse_CJ_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    std::string name;
    auto funct3 = cmd >> 13;
    uint8_t rd;

    if (funct3 == 1) {
        name = "C.JAL";
        rd = 1;
    }
    else if (funct3 == 5) {
        name = "C.J";
        rd = 0;
    }
    else 
        name = "unknown command type CJ";

    char offset;

    // simm[11 | 4 | 9:8 | 10 | 6 | 7 | 3:1 | 5]    
    // head | ten | mid (nine, eight) | seven | six | five | four | tail (three, two, one) 

    uint8_t five = (cmd >> 2) & 0x1;
    uint8_t tail = (cmd >> 3) & 0x7;
    uint8_t seven = (cmd >> 6) & 0x1;
    uint8_t six = (cmd >> 7) & 0x1;
    uint8_t ten = (cmd >> 8) & 0x1;
    uint8_t mid = (cmd >> 9) & 0x3;
    uint8_t four = (cmd >> 10) & 0x1; 
    uint8_t head = (cmd >> 11) & 0x1;

    // мда
    // трэш
    offset = (((((((((((((head << 1) | ten) << 2) | mid) << 1) | seven) << 1) | six) << 1) | five) << 1) | four ) << 3) | tail;
    offset <<= 1; // zero-extended offset

    sprintf(buff, "%08x %10s: %s x%d, %d\n", line, "", name.c_str(), rd, offset);    
    file << buff;
}

void rvc_parsers::parse_CB_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    std::string name;
    auto funct3 = cmd >> 13;
    uint8_t rs1 = (cmd >> 7) & 0x7;
    uint8_t rs2 = 0; // always
    if (funct3 == 6) {
        name = "C.BEQZ";
    }
    else if (funct3 == 7) {
        name = "C.BNEZ";
    }
    else 
        name = "unknown command type CB";


    int16_t offset;
    uint8_t head = (cmd >> 12) & 0x1;
    uint8_t pred_head = (cmd >> 5) & 0x3;
    uint8_t mid = (cmd >> 2) & 0x1;
    uint8_t pred_tail = (cmd >> 10) & 0x3;
    uint8_t tail = (cmd >> 3) & 0x3;
    offset = (((((((head << 2) | pred_head) << 1) | mid) << 2) | pred_tail) << 2) | tail;
    offset <<= 1; // zero extended 

    sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), rs1, rs2, offset);

    file << buff;
}

void rvc_parsers::parse_CR_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];
    uint8_t op = cmd & 0x3;
    std::string name;
    if (op == 1) {
        uint8_t rd = (cmd >> 7) & 0x7;
        uint8_t rs2 = (cmd >> 2) & 0x7;
        uint8_t imm2 = (cmd >> 5) & 0x3;
        uint8_t imm3 = (cmd >> 10) & 0x7;

        if (imm2 == 0 && imm3 == 3) {
            name = "C.SUB";
        } else if (imm2 == 1 && imm3 == 3) {
            name = "C.XOR";
        } else if (imm2 == 2 && imm3 == 3) {
            name = "C.OR";
        } else if (imm2 == 3 && imm3 == 3) {
            name = "C.AND";
        } else if (imm2 == 0 && imm3 == 7) {
            name = "C.SUBW";
        } else if (imm2 == 1 && imm3 == 7) {
            name = "C.ADDW";
        }

        sprintf(buff, "%08x %10s: %s x%d, x%d, x%d\n", line, "", name.c_str(), rd, rd, rs2);
    } else if (op == 2) {
        uint8_t rs2 = (cmd >> 2) & 0x1F;
        if (rs2 != 0) {
            // MV or ADD
            uint8_t rd = (cmd >> 7) & 0x1F;
            uint8_t rs1 = rd;
            if (cmd >> 12 == 8) {
                name = "C.MV";
                rs1 = 2;
            }
            else 
                name = "C.ADD";

            sprintf(buff, "%08x %10s: %s x%d, x%d, x%d\n", line, "", name.c_str(), rd, rs1, rs2);
        } else if (cmd == 0b1001000000000010) {
            sprintf(buff, "%08x %10s %s\n", line, "", "C.EBREAK");
        } else {
            uint8_t rs1 = (cmd >> 7) & 0x1F;
            uint8_t funct4 = (cmd >> 12);
            if (funct4 == 8)
                name = "C.JR";
            else
                name = "C.JALR";
        
            sprintf(buff, "%08x %10s: %s x%d, x%d, %d\n", line, "", name.c_str(), 0, rs1, 0);
        }
    } else
        sprintf(buff, "unknown cmd\n");


    file << buff;
}

void rvc_parsers::parse_CSS_type(uint16_t cmd, std::ofstream& file, int line) {
    char buff[100];

    std::string name;
    auto funct3 = cmd >> 13;
    uint8_t rs2 = (cmd >> 2) & 0xF;
    auto rs1 = 2;

    uint8_t head;
    uint8_t tail; 
    
    if (funct3 == 5)
        name = "C.FSDSP";
    else if (funct3 == 6)
        name = "C.SWSP";
    else if (funct3 == 7)
        name = "FSWSP";
    else 
        name = "unknown name type CSS";

    uint16_t offset;
    if (funct3 == 6 || funct3 == 7) {
        head = (cmd >> 7) & 0x3;
        tail = (cmd >> 9) & 0xF;
        offset = ((head << 4) | tail) << 3;
    } else if (funct3 == 5) {
        head = (cmd >> 7) & 0x7;
        tail = (cmd >> 10) & 0x7;
        offset = ((head << 3) | tail) << 3;
    }

    sprintf(buff, "%08x %10s: %s x%d, %d(x%d)\n", line, "", name.c_str(), rs2, offset, rs1);    
    file << buff;
}
