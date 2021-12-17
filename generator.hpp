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
    void gen_assign(Symbol* sym, long long val);
    void gen_end();
};
