#pragma once

#include <unordered_map>

namespace SymTab {
    namespace Enums {
        enum Bind {
            STB_LOCAL = 0,
            STB_GLOBAL = 1,
            STB_WEAK = 2,
            STB_LOOS = 10,
            STB_HIOS = 12,
            STB_LOPROC = 13,
            STB_HIPROC = 15
        };

        enum Type {
            STT_NOTYPE = 0,
            STT_OBJECT = 1,
            STT_FUNC = 2,
            STT_SECTION = 3,
            STT_FILE = 4,
            STT_COMMON = 5,
            STT_TLS = 6,
            STT_LOOS = 10,
            STT_HIOS = 12,
            STT_LOPROC = 13,
            STT_SPARC_REGISTER = 14,
            STT_HIPROC = 15
        };

        enum Visibility {
            STV_DEFAULT = 0,
            STV_INTERNAL = 1,
            STV_HIDDEN = 2,
            STV_PROTECTED = 3,
            STV_EXPORTED = 4,
            STV_SINGLETON = 5,
            STV_ELIMINATE = 6
        };


        enum SectionIndexes {
            SHN_UNDEF = 0,
            SHN_LORESERVE = 0xff00,
            SHN_LOPROC = 0xff00,
            SHN_BEFORE = 0xff00,
            SHN_AFTER = 0xff01,
            SHN_AMD64_LCOMMON = 0xff02,
            SHN_HIPROC = 0xff1f,
            SHN_LOOS = 0xff20,
            SHN_LOSUNW = 0xff3f,
            SHN_SUNW_IGNORE = 0xff3f,
            SHN_HISUNW = 0xff3f,
            SHN_HIOS = 0xff3f,
            SHN_ABS = 0xfff1,
            SHN_COMMON = 0xfff2,
            SHN_XINDEX = 0xffff,
            SHN_HIRESERVE = 0xffff,
            SHN_TWO = 2 // what?
        };

    };

    namespace Strings {
        extern std::unordered_map<Enums::Bind, const char*> bind;

        extern std::unordered_map<Enums::Type, const char*> type;

        extern std::unordered_map<Enums::SectionIndexes, const char*> indexes;

        extern std::unordered_map<Enums::Visibility, const char*> visibility;
    };

    namespace Getters {
        //using namespace SymTab;

        const char* get_bind(unsigned char info);

        const char* get_type(unsigned char info);

        const char* get_visibility(unsigned char other);

        const char* get_section_index(unsigned short index);
    };

};