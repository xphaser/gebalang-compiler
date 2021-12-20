#pragma once

#include <string>
#include <vector>
#include "symtab.hpp"

using namespace std;

typedef struct lbls {
    long long start;
    long long end;

    lbls(long long start, long long end) {
        this->start = start;
        this->end = end;
    }
} lbls;

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
    void gen_if(lbls* l);
    lbls* gen_if_else(lbls* l);
    lbls* gen_eq(symbol* a, symbol* b);
    lbls* gen_neq(symbol* a, symbol* b);
    lbls* gen_le(symbol* a, symbol* b);
    lbls* gen_ge(symbol* a, symbol* b);
    lbls* gen_leq(symbol* a, symbol* b);
    lbls* gen_geq(symbol* a, symbol* b);
    void gen_while(lbls* l);
    void gen_repeat(long long start, lbls* l);
    void gen_end();
};
