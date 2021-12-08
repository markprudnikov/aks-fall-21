#include "writer.hpp"
#include "symtab.hpp"

void writeTextSection(std::ofstream& file, TextSection& text_sec, const char* str_tab) {
    (void) text_sec;
    (void) str_tab;
    // TODO: parse and write
    file.write("\n", 1);
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

void writer_impl::writeSymbol(std::ofstream& file, Symbol& symbol, const char* str_tab, int index) {
    char str[LINE_SZ];

    sprintf(
        str,
        SYMBOL_FORMAT,
        index,
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