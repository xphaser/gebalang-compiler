#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "symtab.hpp"

using namespace std;

enum instr {
    STORE,
    LOAD,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    GET,
    PUT,
    HALT,
    JUMP,
    JZERO,
    JNZ,
    JPOS,
    JNEG,
    JNPOS,
    JNNEG,
    LABEL
};

typedef struct goto_label {
    int id;
    long long address = -1;

    goto_label(int id) {
        this->id = id;
    }
} goto_label;

typedef struct for_data {
    goto_label* lbl;
    goto_label* jump;
    symbol* temp;

    for_data(goto_label* lbl, goto_label* jump, symbol* temp) {
        this->lbl = lbl;
        this->jump = jump;
        this->temp = temp;
    }
} for_data;

class instruction {
public:
    instr name;
    virtual void render() = 0;
};

class go_to : public instruction {
public:
    goto_label* lbl;

    go_to(instr type, goto_label* lbl) {
        this->name = type;
        this->lbl = lbl;
    }

    void render() override;
};

class nullary : public instruction {
public:
    nullary(instr type) {
        this->name = type;
    }

    void render() override;
};

class unary : public instruction {
public:
    symbol* sym;

    unary(instr type, symbol* sym) {
        this->name = type;
        this->sym = sym;
    }

    void render() override;
};

class binary : public instruction {
public:
    symbol* a;
    symbol* b;

    binary(instr name, symbol* a, symbol* b) {
        this->name = name;
        this->a = a;
        this->b = b;
    }

    void render() override;
};

class intermediate_generator {
public:
    int labels = 0;
    vector<instruction*> instructions;

    void new_add(symbol* a, symbol* b);
    void new_sub(symbol* a, symbol* b);
    void new_mul(symbol* a, symbol* b);
    void new_div(symbol* a, symbol* b);
    void new_mod(symbol* a, symbol* b);
    void new_assign(symbol* sym);
    void new_load(symbol* sym);
    void new_read(symbol* sym);
    void new_write(symbol* sym);
    goto_label* new_eq(symbol* a, symbol* b);
    goto_label* new_neq(symbol* a, symbol* b);
    goto_label* new_ge(symbol* a, symbol* b);
    goto_label* new_le(symbol* a, symbol* b);
    goto_label* new_geq(symbol* a, symbol* b);
    goto_label* new_leq(symbol* a, symbol* b);
    void new_if(goto_label* lbl);
    goto_label* new_if_else(goto_label* lbl);
    goto_label* new_loop_start();
    void new_repeat(goto_label* lbl);
    void new_while(goto_label* start, goto_label* end);
    for_data* new_for_start(symbol* it, symbol* end, symbol* from, symbol* to, bool reverse);
    void new_for_end(for_data* lbls, symbol* it, symbol* one, bool reverse);
    void new_for_end();
    void new_stop();

    void render();
    vector<instruction*> get_instructions();
};
