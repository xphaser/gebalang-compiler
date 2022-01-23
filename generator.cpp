#include "generator.hpp"
#include <iostream>

extern int yyerror(string s);

long long Generator::get_offset() {
    return this->offset;
}

vector<string> Generator::get_code() {
    return this->code;
}

void Generator::append_instr(string instr) {
    this->code.push_back(instr);
    this->offset++;
}

void Generator::gen_end() {
    this->append_instr("HALT");
}

void Generator::gen_const(long long c) {
    bool neg = false;

    if(c<0) {
        c = -c;
        neg = true;
    }
    vector<int> data;

    while(c>0) {
        int i = 0;
        while(c%2 == 0) {
            c/=2;
            i++;
        }
        data.push_back(i);
        c-- ;
    }
    
    this->append_instr("RESET a");
    this->append_instr("RESET b");

    int reg = 0;

    while(!data.empty()) {
        int i = data.back();
        data.pop_back();

        if(neg)
            this->append_instr("DEC a");
        else
            this->append_instr("INC a");
        
        if(i) {
            if(reg < i) {
                do {
                    this->append_instr("INC b");
                    reg++;
                } while(reg < i);
            }
            else if(reg > i) {
                do {
                    this->append_instr("DEC b");
                    reg--;
                } while(reg > i);
            }
            this->append_instr("SHIFT b");
        }
    }
}

void Generator::get_value(symbol* sym) {
    if(sym->is_iterator) {
        sym->initialized = true;
    }
    if(sym->relation >= 0) {
        this->gen_const(sym->relation);    //get adress (of index)
        this->append_instr("LOAD a");      //get value of index
        this->append_instr("SWAP c");      //swap index to c
        this->gen_const(sym->offset - sym->array_start);  //get offset
        this->append_instr("ADD c");       //add index to offset
        this->append_instr("LOAD a");
    }
    /*else if(!sym->initialized) {
        sym->initialized = true;

        if(sym->is_const) {
            //this->gen_address(sym->offset);
            this->gen_const(stoll(sym->name));
            //this->append_instr("STORE d");
        }
        else {
            yyerror("variable \e[0;1m‘" + sym->name + "’\e[0m used before being initialized");
        }
    }*/
    else {
        if(sym->is_const) {
            this->gen_const(stoll(sym->name));
        }
        else {
            this->gen_const(sym->offset);
            this->append_instr("LOAD a");
        }
    }
}

void Generator::gen_address(long long addr) {
    this->gen_const(addr);
    this->append_instr("SWAP d");
}

void Generator::gen_assign(symbol* sym) {
    sym->initialized = true;
    this->append_instr("SWAP d");
    if(sym->relation >= 0) {
        this->gen_const(sym->relation);    //get adress (of index)
        this->append_instr("LOAD a");      //get value of index
        this->append_instr("SWAP c");      //swap index to c
        this->gen_const(sym->offset - sym->array_start);  //get offset
        this->append_instr("ADD c");       //add index to offset
        this->append_instr("SWAP d");
    }
    else {
        this->gen_address(sym->offset);
    }
    this->append_instr("STORE d");
}

void Generator::gen_read(symbol* sym) {
    this->append_instr("GET");
    this->gen_assign(sym);
}

void Generator::gen_write(symbol* sym) {
    this->get_value(sym);
    this->append_instr("PUT");
}

void Generator::gen_add(symbol* a, symbol* b) {
    this->get_value(b);
    this->append_instr("SWAP e");
    this->get_value(a);
    this->append_instr("ADD e");
}

void Generator::gen_sub(symbol* a, symbol* b) {
    this->get_value(b);
    this->append_instr("SWAP e");
    this->get_value(a);
    this->append_instr("SUB e");
}

void Generator::gen_mult(symbol* a, symbol* b) {
    if(a->is_const && b->is_const) {
        long long c = stoll(a->name) * stoll(b->name);
        this->gen_const(c);
    }
    else {
        this->get_value(b);
        this->append_instr("SWAP d");
        this->get_value(a);
        this->append_instr("RESET h");
        this->append_instr("JNEG 2");
        this->append_instr("JUMP 5");
        this->append_instr("RESET c");
        this->append_instr("SWAP c");
        this->append_instr("SUB c");
        this->append_instr("INC h");
        this->append_instr("RESET g");
        this->append_instr("JPOS 2");
        this->append_instr("JUMP ");
        long long start = this->get_offset();
        this->append_instr("RESET e");
        this->append_instr("INC e");;
        this->append_instr("SWAP f");
        this->append_instr("RESET a");
        this->append_instr("INC a");
        this->append_instr("SHIFT e");
        this->append_instr("SUB f");
        this->append_instr("JPOS 3");
        this->append_instr("INC e");
        this->append_instr("JUMP -6");
        this->append_instr("DEC e");
        this->append_instr("RESET a");
        this->append_instr("ADD d");
        this->append_instr("SHIFT e");
        this->append_instr("SWAP g");
        this->append_instr("ADD g");
        this->append_instr("SWAP g");
        this->append_instr("RESET a");
        this->append_instr("INC a");
        this->append_instr("SHIFT e");
        this->append_instr("SWAP f");
        this->append_instr("SUB f");
        this->append_instr("JUMP " + to_string(start - this->get_offset() - 2));
        this->code[start - 1] += to_string(this->get_offset() - start + 1);
        this->append_instr("SWAP h");
        this->append_instr("JZERO 4");
        this->append_instr("RESET a");
        this->append_instr("SUB g");
        this->append_instr("JUMP 2");
        this->append_instr("SWAP g");
    }
}

void Generator::gen_div(symbol* a, symbol* b) {
    if(a->is_const && b->is_const) {
        long long c = stoll(a->name) / stoll(b->name);
        this->gen_const(c);
    }
    else {
        this->get_value(b);
        long long addr = this->get_offset();
        this->append_instr("JZERO ");
        this->append_instr("RESET h");
        this->append_instr("JPOS 5");
        this->append_instr("INC h");
        this->append_instr("SWAP f");
        this->append_instr("RESET a");
        this->append_instr("SUB f");
        this->append_instr("SWAP f");
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SWAP d");
        this->get_value(a);
        this->append_instr("JNEG 2");
        this->append_instr("JUMP 5");
        this->append_instr("DEC h");
        this->append_instr("SWAP e");
        this->append_instr("RESET a");
        this->append_instr("SUB e");
        this->append_instr("SWAP e");
        this->append_instr("RESET b");
        this->append_instr("INC b");
        this->append_instr("RESET g");
        this->append_instr("RESET a");
        this->append_instr("ADD e");
        this->append_instr("JZERO 29");
        this->append_instr("SUB f");
        this->append_instr("JZERO 26");
        this->append_instr("JNEG 31");
        this->append_instr("RESET c");
        this->append_instr("INC c");
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SHIFT b");
        this->append_instr("SUB e");
        this->append_instr("JPOS 8");
        this->append_instr("SWAP f");
        this->append_instr("SHIFT b");
        this->append_instr("SWAP f");
        this->append_instr("SWAP c");
        this->append_instr("SHIFT b");
        this->append_instr("SWAP c");
        this->append_instr("JUMP -11");
        this->append_instr("SWAP g");
        this->append_instr("ADD c");
        this->append_instr("SWAP g");
        this->append_instr("SWAP e");
        this->append_instr("SUB f");
        this->append_instr("SWAP e");
        this->append_instr("RESET a");
        this->append_instr("ADD d");
        this->append_instr("SWAP f");
        this->append_instr("JUMP -29");
        this->append_instr("INC g");
        this->append_instr("SWAP h");
        this->append_instr("JZERO 10");
        this->append_instr("RESET a");
        this->append_instr("SUB g");
        this->append_instr("JUMP 8");
        this->append_instr("SWAP h");
        this->append_instr("JZERO 5");
        this->append_instr("RESET a");
        this->append_instr("SUB g");
        this->append_instr("DEC a");
        this->append_instr("JUMP 2");
        this->append_instr("SWAP g");
        this->code[addr] += to_string(this->get_offset() - addr);
    }
}

void Generator::gen_mod(symbol* a, symbol* b) {
    if(a->is_const && b->is_const) {
        long long c = stoll(a->name) % stoll(b->name);
        this->gen_const(c);
    }
    else {
        this->get_value(b);
        long long addr = this->get_offset();
        this->append_instr("JZERO ");
        this->append_instr("RESET h");
        this->append_instr("RESET g");
        this->append_instr("JPOS 6");
        this->append_instr("INC g");
        this->append_instr("INC h");
        this->append_instr("SWAP f");
        this->append_instr("RESET a");
        this->append_instr("SUB f");
        this->append_instr("SWAP f");
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SWAP d");
        this->get_value(a);
        this->append_instr("JNEG 2");
        this->append_instr("JUMP 5");
        this->append_instr("DEC h");
        this->append_instr("SWAP e");
        this->append_instr("RESET a");
        this->append_instr("SUB e");
        this->append_instr("SWAP e");
        this->append_instr("RESET b");
        this->append_instr("INC b");
        this->append_instr("RESET a");
        this->append_instr("ADD e");
        this->append_instr("SUB f"); 
        this->append_instr("JZERO 19");
        this->append_instr("JNEG 17");
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SHIFT b");
        this->append_instr("SUB e");
        this->append_instr("JPOS 5");
        this->append_instr("SWAP f");
        this->append_instr("SHIFT b");
        this->append_instr("SWAP f");
        this->append_instr("JUMP -8");
        this->append_instr("SWAP e");
        this->append_instr("SUB f");
        this->append_instr("SWAP e");
        this->append_instr("RESET a");
        this->append_instr("ADD d");
        this->append_instr("SWAP f");
        this->append_instr("JUMP -20");
        this->append_instr("SWAP e");
        this->append_instr("JZERO 14");
        this->append_instr("SWAP e");
        this->append_instr("SWAP h");
        this->append_instr("JZERO 5"); 
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SUB e");
        this->append_instr("SWAP e");
        this->append_instr("SWAP g");
        this->append_instr("JZERO 4");
        this->append_instr("RESET a");
        this->append_instr("SUB e");
        this->append_instr("JUMP 2");
        this->append_instr("SWAP e");
        this->code[addr] += to_string(this->get_offset() - addr);
    }
}

void Generator::gen_if(lbls* l) {
    this->code[l->end] += to_string(this->offset - l->end);
}

lbls* Generator::gen_if_else(lbls* l) {
    this->append_instr("JUMP ");
    this->gen_if(l);
    l->end = this->offset - 1;
    return l;
}

lbls* Generator::gen_eq(symbol* a, symbol* b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JZERO 2");
    this->append_instr("JUMP ");
    long long end = this->offset - 1;
    cout << start <<" "<<end<<endl;
    return new lbls(start, end);
}

lbls* Generator::gen_neq(symbol* a, symbol *b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JZERO ");
    long long end = this->offset - 1;

    return new lbls(start, end);
}

lbls* Generator::gen_le(symbol* a, symbol *b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JNEG 2");
    this->append_instr("JUMP ");
    long long end = this->offset - 1;

    return new lbls(start, end);
}

lbls* Generator::gen_ge(symbol* a, symbol *b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JPOS 2");
    this->append_instr("JUMP ");
    long long end = this->offset - 1;

    return new lbls(start, end);
}

lbls* Generator::gen_leq(symbol* a, symbol *b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JPOS ");
    long long end = this->offset - 1;

    return new lbls(start, end);
}

lbls* Generator::gen_geq(symbol* a, symbol *b) {
    long long start = this->offset;
    this->gen_sub(a, b);
    this->append_instr("JNEG ");
    long long end = this->offset - 1;

    return new lbls(start, end);
}

void Generator::gen_while(lbls* l) {
    this->append_instr("JUMP " + to_string(l->start - this->offset));
    this->code[l->end] += to_string(this->offset - l->end);
}

void Generator::gen_repeat(long long start, lbls* l) {
    this->code[l->end] += to_string(start - l->end);
}

forlbl* Generator::gen_for_start(symbol* iterator, symbol* from, symbol* to, bool reverse) {
    string tmp = to->name + "tmp";
    this->symbols->putsym(tmp);
    symbol *to_tmp = symbols->getsym(tmp);
    to_tmp->initialized = true;
    this->gen_address(to_tmp->offset);
    this->get_value(to);
    this->append_instr("STORE d");

    this->get_value(from);
    this->append_instr("SWAP d");
    this->gen_address(iterator->offset);
    this->append_instr("STORE d");
    lbls* jump = new lbls(this->offset, 0);
    this->append_instr("SWAP d");
    this->get_value(to_tmp);
    this->append_instr("SUB d");
    
    jump->end = this->offset + 1;
    if(reverse) {
        this->append_instr("JPOS ");
    }
    else {
        this->append_instr("JNEG ");
    }

    return new forlbl(iterator, from, to_tmp, jump);
}

void Generator::gen_for_end(forlbl* label, bool reverse) {
    this->gen_address(label->iterator->offset);
    this->append_instr("LOAD d");
    if(reverse) this->append_instr("DEC a");
    else this-> append_instr("INC a");
    this->append_instr("STORE d");

    this->append_instr("JUMP " + to_string(label->jump->start - this->offset));
    this->code[label->jump->end - 1] += to_string(this->offset - label->jump->end + 1);
}
