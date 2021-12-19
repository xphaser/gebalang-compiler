#pragma once

#include <string>
#include <vector>
#include "symtab.hpp"

using namespace std;

class Generator {
    long long offset;
    vector<string> code;
    Symtab* symbols;

public:
    Generator(Symtab* symbols) {
        this->symbols = symbols;
        this->offset = 0;
    }

    long long get_offset();
    vector<string> get_code();
    void append_instr(string instr);
    void gen_read(symbol* sym);
    void gen_write(symbol* sym);
    void gen_assign(symbol* sym);
    void get_value(symbol* sym);
    void gen_const(long long c);
    void gen_address(long long addr);
    void gen_add(symbol* a, symbol* b);
    void gen_sub(symbol* a, symbol* b);
    void gen_mult(symbol* a, symbol* b);
    void gen_div(symbol* a, symbol* b);
    void gen_mod(symbol* a, symbol* b);
    void gen_end();
};
