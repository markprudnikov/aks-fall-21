#include "writer.hpp"
#include "symtab.hpp"
#include "disassemble.hpp"

#include <iostream>

void writeByType(Types type, uint32_t cmd, std::ofstream& file, int line) {
    switch (type) {
        case R:
            type_parsers::parse_R_type(cmd, file, line);
            break;
        case U:
            type_parsers::parse_U_type(cmd, file, line);
            break;
        case S:
            type_parsers::parse_S_type(cmd, file, line);
            break;
        case I:
            type_parsers::parse_I_type(cmd, file, line);
            break;
        case IL:
            type_parsers::parse_IL_type(cmd, file, line);
            break;
        case J:
            type_parsers::parse_J_type(cmd, file, line);
            break;
        case B:
            type_parsers::parse_B_type(cmd, file, line);
            break;
        default:
            file << "unknown type\n";
    }
}

void writeByQuadrant(Quadrants q, uint16_t cmd, std::ofstream& file, int line) {
    switch (q) {
        case Zero:
            quadrant_parsers::parse_Zero_Quad(cmd, file, line);
            break;
        case One:
            quadrant_parsers::parse_One_Quad(cmd, file, line);
            break;
        case Two:
            quadrant_parsers::parse_Two_Quad(cmd, file, line);
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
        } else if (is16BitCmd(text_sec[i])) {
            uint16_t cmd = text_sec[i];
            Quadrants q = get_quadrant(text_sec[i]);
            writeByQuadrant(q, cmd, file, line);
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