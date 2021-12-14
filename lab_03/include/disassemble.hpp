#ifndef LAB_03_DISASSEMBLE_HPP
#define LAB_03_DISASSEMBLE_HPP

#include "elf_parser.hpp"

// IL -- Type I, Load instructions
enum Types {
    R, I, S, B, U, J, IL
};

enum Quadrants {
    Zero, One, Two
};

Types get_type(uint32_t cmd);

Quadrants get_quadrant(uint16_t cmd);

namespace type_parsers {
    void parse_R_type(uint32_t cmd, std::ostream& file, int line);
    void parse_U_type(uint32_t cmd, std::ostream& file, int line);
    void parse_S_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_J_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_I_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_IL_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_B_type(uint32_t cmd, std::ostream& file, int line);
}

namespace quadrant_parsers {
    void parse_Zero_Quad(uint16_t cmd, std::ofstream& file, int line);
    void parse_One_Quad(uint16_t cmd, std::ofstream& file, int line);
    void parse_Two_Quad(uint16_t cmd, std::ofstream& file, int line);
}

unsigned short shift(Parcel parcel);

bool is32BitCmd(Parcel parcel);

bool is16BitCmd(Parcel parcel);

#endif //LAB_03_DISASSEMBLE_HPP
