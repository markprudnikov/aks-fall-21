#include "symtab.hpp"

using namespace SymTab;

const char* Getters::get_bind(unsigned char info) {
    info >>= 4;

    switch (info) {
        case Enums::Bind::STB_LOCAL:
            return "LOCAL";
        case Enums::Bind::STB_GLOBAL:
            return "GLOBAL";
        case Enums::Bind::STB_WEAK:
            return "WEAK";
        default:
            return "UNKNOWN";
    }
}

const char* Getters::get_type(unsigned char info) {
    info &= 0xF;

    switch (info) {
        case Enums::Type::STT_NOTYPE:
            return "NOTYPE";
        case Enums::Type::STT_OBJECT:
            return "OBJECT";
        case Enums::Type::STT_FUNC:
            return "FUNC";
        case Enums::Type::STT_SECTION:
            return "SECTION";
        case Enums::Type::STT_FILE:
            return "FILE";
        case Enums::Type::STT_COMMON:
            return "COMMON";
        case Enums::Type::STT_TLS:
            return "TLS";
        default:
            return "UNKNOWN";
    }
}

const char* Getters::get_visibility(unsigned char other) {
    other &= 0x3;

    switch (other) {
        case Enums::Visibility::STV_DEFAULT:
            return "DEFAULT";
        case Enums::Visibility::STV_INTERNAL:
            return "INTERNAL";
        case Enums::Visibility::STV_HIDDEN:
            return "HIDDEN";
        case Enums::Visibility::STV_PROTECTED:
            return "PROTECTED";
        case Enums::Visibility::STV_EXPORTED:
            return "EXPORTED";
        case Enums::Visibility::STV_SINGLETON:
            return "SINGLETON";
        case Enums::Visibility::STV_ELIMINATE:
            return "ELIMINATE";
        default:
            return "UNKNOWN";
    }

}

const char* Getters::get_section_index(unsigned short index) {
    switch (index) {
        case Enums::Ndx::SHN_UND:
            return "UND";
        case Enums::Ndx::SHN_ABS:
            return "ABS";
        case Enums::Ndx::SHN_COMMON:
            return "COMMON";
        default:
            static char def[2];
            sprintf(def, "%hu", index);
            return def;
    }
}
