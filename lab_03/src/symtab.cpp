#include "symtab.hpp"

using namespace SymTab;

std::unordered_map<Enums::Bind, const char*> Strings::bind = {
        {Enums::Bind::STB_LOCAL, "LOCAL"},
        {Enums::Bind::STB_GLOBAL, "GLOBAL"},
        {Enums::Bind::STB_WEAK, "WEAK"},
        {Enums::Bind::STB_LOOS, "LOOS"},
        {Enums::Bind::STB_HIOS, "HIOS"},
        {Enums::Bind::STB_LOPROC, "LOPROC"},
        {Enums::Bind::STB_HIPROC, "HIPROC"}
};

std::unordered_map<Enums::Type, const char*> Strings::type = {
    {Enums::Type::STT_NOTYPE, "NOTYPE"},
    {Enums::Type::STT_OBJECT, "OBJECT"},
    {Enums::Type::STT_FUNC, "FUNC"},
    {Enums::Type::STT_SECTION, "SECTION"},
    {Enums::Type::STT_FILE, "FILE"},
    {Enums::Type::STT_COMMON, "COMMON"},
    {Enums::Type::STT_TLS, "TLS"},
    {Enums::Type::STT_LOOS, "LOOS"},
    {Enums::Type::STT_HIOS, "HIOS"},
    {Enums::Type::STT_LOPROC, "LOPROC"},
    {Enums::Type::STT_SPARC_REGISTER, "SPARC_REGISTER"},
    {Enums::Type::STT_HIPROC, "HIPROC"}
};

std::unordered_map<Enums::SectionIndexes, const char*> Strings::indexes = {
    {Enums::SectionIndexes::SHN_TWO, "2"}, // what?
    {Enums::SectionIndexes::SHN_UNDEF, "UND"},
    {Enums::SectionIndexes::SHN_LORESERVE, "LORESERVE"},
    {Enums::SectionIndexes::SHN_LOPROC, "LOPROC"},
    {Enums::SectionIndexes::SHN_BEFORE, "BEFORE"},
    {Enums::SectionIndexes::SHN_AFTER, "AFTER"},
    {Enums::SectionIndexes::SHN_AMD64_LCOMMON, "AMD64_LCOMMON"},
    {Enums::SectionIndexes::SHN_HIPROC, "HIPROC"},
    {Enums::SectionIndexes::SHN_LOOS, "LOOS"},
    {Enums::SectionIndexes::SHN_LOSUNW, "LOSUNW"},
    {Enums::SectionIndexes::SHN_SUNW_IGNORE, "SUNW_IGNORE"},
    {Enums::SectionIndexes::SHN_HISUNW, "HISUNW"},
    {Enums::SectionIndexes::SHN_HIOS, "HIOS"},
    {Enums::SectionIndexes::SHN_ABS, "ABS"},
    {Enums::SectionIndexes::SHN_COMMON, "COMMON"},
    {Enums::SectionIndexes::SHN_XINDEX, "XINDEX"},
    {Enums::SectionIndexes::SHN_HIRESERVE, "HIRESERVE"}
};

std::unordered_map<Enums::Visibility, const char*> Strings::visibility = {
    {Enums::Visibility::STV_DEFAULT, "DEFAULT"},
    {Enums::Visibility::STV_INTERNAL, "INTERNAL"},
    {Enums::Visibility::STV_HIDDEN, "HIDDEN"},
    {Enums::Visibility::STV_PROTECTED, "PROTECTED"},
    {Enums::Visibility::STV_EXPORTED, "EXPORTED"},
    {Enums::Visibility::STV_SINGLETON, "SINGLETON"},
    {Enums::Visibility::STV_ELIMINATE, "ELIMINATE"}
};


const char* Getters::get_bind(unsigned char info) {
    return Strings::bind[static_cast<Enums::Bind>(info >> 4)];
}

const char* Getters::get_type(unsigned char info) {
    return Strings::type[static_cast<Enums::Type>(info & 0xF)];
}

const char* Getters::get_visibility(unsigned char other) {
    return Strings::visibility[static_cast<Enums::Visibility>(other & 0x3)];
}

const char* Getters::get_section_index(unsigned short index) {
    return Strings::indexes[static_cast<Enums::SectionIndexes>(index)];
};
