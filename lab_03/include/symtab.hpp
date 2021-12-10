#pragma once

#include <unordered_map>

namespace SymTab {
    namespace Enums {
        enum Bind {
            STB_LOCAL = 0,
            STB_GLOBAL = 1,
            STB_WEAK = 2
        };

        enum Type {
            STT_NOTYPE = 0,
            STT_OBJECT = 1,
            STT_FUNC = 2,
            STT_SECTION = 3,
            STT_FILE = 4,
            STT_COMMON = 5,
            STT_TLS = 6
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


        enum Ndx {
            SHN_UND = 0,
            SHN_ABS = 0xfff1,
            SHN_COMMON = 0xfff2,
        };

    }

    namespace Getters {
        const char* get_bind(unsigned char info);

        const char* get_type(unsigned char info);

        const char* get_visibility(unsigned char other);

        const char* get_section_index(unsigned short index);
    }

}