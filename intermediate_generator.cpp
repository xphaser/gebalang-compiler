#include "intermediate_generator.hpp"
#include <iostream>

using namespace std;

void intermediate_generator::new_add(symbol* a, symbol* b) {
    instructions.push_back(new binary(ADD, a, b));
}

void intermediate_generator::new_sub(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
}

void intermediate_generator::new_mul(symbol* a, symbol* b) {
    instructions.push_back(new binary(MUL, a, b));
}

void intermediate_generator::new_div(symbol* a, symbol* b) {
    instructions.push_back(new binary(DIV, a, b));
}

void intermediate_generator::new_mod(symbol* a, symbol* b) {
    instructions.push_back(new binary(MOD, a, b));
}

void intermediate_generator::new_assign(symbol* sym) {
    instructions.push_back(new unary(STORE, sym));
}

void intermediate_generator::new_load(symbol* sym) {
    instructions.push_back(new unary(LOAD, sym));
}

void intermediate_generator::new_read(symbol* sym) {
    instructions.push_back(new nullary(GET));
    instructions.push_back(new unary(STORE, sym));
}

void intermediate_generator::new_write(symbol* sym) {
    instructions.push_back(new unary(LOAD, sym));
    instructions.push_back(new nullary(PUT));
}

void intermediate_generator::new_if(goto_label* lbl) {
    instructions.push_back(new go_to(LABEL, lbl));
}

goto_label* intermediate_generator::new_loop_start() {
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(LABEL, lbl));
    return lbl;
}

void intermediate_generator::new_while(goto_label* start, goto_label* end) {
    instructions.push_back(new go_to(JUMP, start));
    instructions.push_back(new go_to(LABEL, end));
}

void intermediate_generator::new_repeat(goto_label* lbl) {
    go_to* jump = (go_to*)instructions.back();
    jump->lbl = lbl;
}

for_data* intermediate_generator::new_for_start(symbol* it, symbol* end, symbol* from, symbol* to, bool reverse) {
    goto_label* lbl = new goto_label(++this->labels);
    goto_label* jump = new goto_label(++this->labels);

    instructions.push_back(new unary(LOAD, to));
    instructions.push_back(new unary(STORE, end));
    instructions.push_back(new unary(LOAD, from));
    instructions.push_back(new unary(STORE, it));
    instructions.push_back(new go_to(LABEL, lbl));
    instructions.push_back(new binary(SUB, end, it));
    if(reverse)
        instructions.push_back(new go_to(JPOS, jump));
    else
        instructions.push_back(new go_to(JNEG, jump));

    return new for_data(lbl, jump, end);
}

void intermediate_generator::new_for_end(for_data* lbls, symbol* it, symbol* one, bool reverse) {
    if(reverse)
        instructions.push_back(new binary(SUB, it, one));
    else
        instructions.push_back(new binary(ADD, it, one));

    instructions.push_back(new unary(STORE, it));
    instructions.push_back(new go_to(JUMP, lbls->lbl));
    instructions.push_back(new go_to(LABEL, lbls->jump));
}

goto_label* intermediate_generator::new_if_else(goto_label* lbl) {
    goto_label* end = new goto_label(++this->labels);
    instructions.push_back(new go_to(JUMP, end));
    instructions.push_back(new go_to(LABEL, lbl));
    return end;
}

goto_label* intermediate_generator::new_eq(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JNZ, lbl));
    return lbl;
}

goto_label* intermediate_generator::new_neq(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JZERO, lbl));
    return lbl;
}

goto_label* intermediate_generator::new_ge(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JNPOS, lbl));
    return lbl;
}

goto_label* intermediate_generator::new_le(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JNNEG, lbl));
    return lbl;
}

goto_label* intermediate_generator::new_geq(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JNEG, lbl));
    return lbl;
}

goto_label* intermediate_generator::new_leq(symbol* a, symbol* b) {
    instructions.push_back(new binary(SUB, a, b));
    goto_label* lbl = new goto_label(++this->labels);
    instructions.push_back(new go_to(JPOS, lbl));
    return lbl;
}

void intermediate_generator::new_stop() {
    instructions.push_back(new nullary(HALT));
}

vector<instruction*> intermediate_generator::get_instructions() {
     return this->instructions;
}

void intermediate_generator::render() {
    for(int i=0; i<instructions.size(); i++) {
        instructions[i]->render();
    }
}

void go_to::render() {
    switch (name) {
    case JUMP:
        cout << "\tgoto";
        break;
    case JZERO:
        cout << "\tif == 0 goto";
        break;
    case JNZ:
        cout << "\tif != 0 goto";
        break;
    case JNEG:
        cout << "\tif < 0 goto";
        break;
    case JPOS:
        cout << "\tif > 0 goto";
        break;
    case JNNEG:
        cout << "\tif >= 0 goto";
        break;
    case JNPOS:
        cout << "\tif <= 0 goto";
    }
    cout << " L" << to_string(lbl->id);

    if (name == LABEL) {
        cout << ":";
    }
    cout << endl;
}

void nullary::render() {
    if(name == GET) {
        cout << "\tget" << endl;
    }
    else if(name == PUT) {
        cout << "\tput" << endl;
    }
}

void unary::render() {
    if(name == STORE) {
        cout << "\t" << sym->name << " := tmp" << endl;
    }
    else if(name == LOAD) {
        cout << "\ttmp := " << sym->name << endl;
    }
}

void binary::render() {
    string op;

    switch (name){
    case ADD:
        op = " + ";
        break;
    case SUB:
        op = " - ";
        break;
    case MUL:
        op = " * ";
        break;
    case DIV:
        op = " / ";
        break;
    case MOD:
        op = " % ";
        break;
    }
    cout << "\ttmp = " << a->name << op << b->name << endl;
}
