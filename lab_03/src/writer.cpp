#include "writer.hpp"
#include "symtab.hpp"


#include <iostream>

void writeByType(RV32_Types type, uint32_t cmd, std::ofstream& file, int line, const char* mark) {
    switch (type) {
        case R:
            rv32_parsers::parse_R_type(cmd, file, line, mark);
            break;
        case U:
            rv32_parsers::parse_U_type(cmd, file, line, mark);
            break;
        case S:
            rv32_parsers::parse_S_type(cmd, file, line, mark);
            break;
        case I:
            rv32_parsers::parse_I_type(cmd, file, line, mark);
            break;
        case IL:
            rv32_parsers::parse_IL_type(cmd, file, line, mark);
            break;
        case J:
            rv32_parsers::parse_J_type(cmd, file, line, mark);
            break;
        case B:
            rv32_parsers::parse_B_type(cmd, file, line, mark);
            break;
        default:
            file << "unknown type\n";
    }
}

std::string writer_impl::get_mark(int line, SymbolTable& symtab, const char* str_tab, int text_index) {
    for (auto s : symtab) {
        if (s.st_value == line && s.st_shndx == text_index)
            return std::string(str_tab + s.st_name) + ":";
    }

    return "";
}

void writeRVCbyType(RVC_Types type, uint16_t cmd, std::ofstream& file, int line, const char* mark) {
    switch (type) { 
        case CIW:
            rvc_parsers::parse_CIW_type(cmd, file, line, mark);
            break;
        case CI:
            rvc_parsers::parse_CI_type(cmd, file, line, mark);
            break;
        case CL:
        case CS:
            rvc_parsers::parse_CL_CS_types(cmd, file, line, mark);
            break;
        case CSS:
            rvc_parsers::parse_CSS_type(cmd, file, line, mark);
            break;
        case CJ:
            rvc_parsers::parse_CJ_type(cmd, file, line, mark);
            break;
        case CB:
            rvc_parsers::parse_CB_type(cmd, file, line, mark);
            break;
        case CR:
            rvc_parsers::parse_CR_type(cmd, file, line, mark);
            break;
        case UNKWN:
        default:
            file << "unknown type of 16 bit cmd\n";
            
    }
}

void writeTextSection(std::ofstream& file, TextSection& text_sec, SymbolTable& symtab, const char* str_tab, int text_index) {
    int line = 0;
    
    for (std::size_t i = 0; i < text_sec.size(); ++i) {
        std::string mark = writer_impl::get_mark(line, symtab, str_tab, text_index);
        if (is32BitCmd(text_sec[i])) {
            uint16_t tail = text_sec[i];
            uint16_t head = text_sec[++i];
            uint32_t cmd = (head << 16) | tail;
            RV32_Types type = get_type(cmd);
            writeByType(type, cmd, file, line, mark.c_str());
            line += 4;
        } else if (is16BitCmd(text_sec[i])) {
            uint16_t cmd = text_sec[i];
            RVC_Types q = get_type(text_sec[i]);
            writeRVCbyType(q, cmd, file, line, mark.c_str());
            line += 2;
        } else {
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
    char str[LINE_SZ] {'\0'};

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