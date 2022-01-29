#pragma once

#include <string>
#include <vector>
#include "symtab.hpp"
#include "intermediate_generator.hpp"

using namespace std;

class target_generator {
    long long offset = 0;
    vector<string> code;

public:
    long long get_offset();
    void generate_code(vector<instruction*> instructions);
    vector<string> get_code();
    void append_instr(string instr);
    void backpatch(long long address, long long value);
    void gen_get();
    void gen_put();
    void gen_end();
    void gen_assign(symbol* sym);
    void get_value(symbol* sym);
    void gen_const(long long c);
    void gen_address(long long addr);
    void gen_add(symbol* a, symbol* b);
    void gen_sub(symbol* a, symbol* b);
    void gen_mul(symbol* a, symbol* b);
    void gen_div(symbol* a, symbol* b);
    void gen_mod(symbol* a, symbol* b);
    void gen_jump(goto_label* lbl);
    void gen_jzero(goto_label* lbl);
    void gen_jnz(goto_label* lbl);
    void gen_jpos(goto_label* lbl);
    void gen_jneg(goto_label* lbl);
    void gen_jnpos(goto_label* lbl);
    void gen_jnneg(goto_label* lbl);
};
