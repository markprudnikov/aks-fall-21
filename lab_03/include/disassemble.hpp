#pragma one

#include "elf_parser.hpp"

// IL -- Type I, Load instructions
enum RV32_Types {
    R, I, S, B, U, J, IL
};

enum RVC_Types {
    CIW, CL, CS, CI, CJ, CB, CR, CSS, UNKWN
};

RV32_Types get_type(uint32_t cmd);

RVC_Types get_type(uint16_t cmd);

namespace rv32_parsers {
    void parse_R_type(uint32_t cmd, std::ostream& file, int line);
    void parse_U_type(uint32_t cmd, std::ostream& file, int line);
    void parse_S_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_J_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_I_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_IL_type(uint32_t cmd, std::ofstream& file, int line);
    void parse_B_type(uint32_t cmd, std::ostream& file, int line);
}

namespace rvc_parsers {
    void parse_CIW_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CL_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CS_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CI_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CJ_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CB_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CR_type(uint16_t cmd, std::ofstream& file, int line);
    void parse_CSS_type(uint16_t cmd, std::ofstream& file, int line);
}

unsigned short shift(Parcel parcel);

bool is32BitCmd(Parcel parcel);

bool is16BitCmd(Parcel parcel);

