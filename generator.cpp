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
    this->append_instr("RESET a");
    long long k = 0;

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
    if(!sym->initialized) {
        sym->initialized = true;

        if(sym->is_const) {
            //this->gen_address(sym->offset);
            this->gen_const(stoll(sym->name));
            //this->append_instr("STORE d");
        }
        else {
            yyerror("variable \e[0;1m‘" + sym->name + "’\e[0m used before being initialized");
        }
    }
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
    this->gen_address(sym->offset);
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
        this->append_instr("RESET g"); //res=0
        this->get_value(a);
        this->append_instr("JPOS 2");  //a>0
        this->append_instr("JUMP ");   //outside while
        long long start = this->get_offset();
        this->append_instr("RESET e"); //x=0
        this->append_instr("INC e");   //x++;
        this->append_instr("SWAP f");  //a to regF
        this->append_instr("RESET a");
        this->append_instr("INC a");   //one
        this->append_instr("SHIFT e"); //1<<x+1
        this->append_instr("SUB f");
        this->append_instr("JPOS 3");  //outside second while
        this->append_instr("INC e");   //x++
        this->append_instr("JUMP -6");
        this->append_instr("DEC e");   //x--
        this->get_value(b);
        this->append_instr("SHIFT e"); //b<<x
        this->append_instr("SWAP g");  //b<<x to regG  res from regG to ac
        this->append_instr("ADD g");   //res+=b<<x
        this->append_instr("SWAP g");  //res to regG
        this->append_instr("RESET a");
        this->append_instr("INC a");   //one
        this->append_instr("SHIFT e"); //1<<x
        this->append_instr("SWAP f");  //a to ac    1<<x to regF
        this->append_instr("SUB f");   //a-= 1<<x
        this->append_instr("JUMP " + to_string(start - this->get_offset() - 2));
        this->code[start - 1] += to_string(this->get_offset() - start + 1); //backpatch
        this->append_instr("SWAP g");  //res to ac
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
        this->append_instr("SWAP f");  //b to regF
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SWAP d");
        this->get_value(a);
        this->append_instr("SWAP e");  //a to regE
        this->append_instr("RESET b");
        this->append_instr("INC b");   //one;
        this->append_instr("RESET g"); //res=0;

        this->append_instr("RESET a");
        this->append_instr("ADD e");   //ac=a
        this->append_instr("SUB f");   //ac=a-b
        this->append_instr("JZERO 26"); //somewhere lol
        this->append_instr("JNEG 26");
        this->append_instr("RESET c"); //q=0;
        this->append_instr("INC c");   //q=1;
        //while
        this->append_instr("RESET a");
        this->append_instr("ADD f");   //cpy b to ac
        this->append_instr("SHIFT b"); //b=b<<1;
        this->append_instr("SUB e");   //
        this->append_instr("JPOS 8");  //b<<1 > a
        this->append_instr("SWAP f");  //swap b to ac
        this->append_instr("SHIFT b"); //b<<1
        this->append_instr("SWAP f");  //b from ac to regF
        this->append_instr("SWAP c");  //q to ac
        this->append_instr("SHIFT b"); //q<<1;
        this->append_instr("SWAP c");  //q to regC
        this->append_instr("JUMP -11");
        this->append_instr("SWAP g");  //res to ac
        this->append_instr("ADD c");   //res+=q;
        this->append_instr("SWAP g");  //res to regD;
        this->append_instr("SWAP e");  //a to ac
        this->append_instr("SUB f");   //a=a-b
        this->append_instr("SWAP e");  //a to regE
        this->append_instr("RESET a");
        this->append_instr("ADD d");
        this->append_instr("SWAP f");
        this->append_instr("JUMP -28");

        //if a==b
        this->append_instr("INC g");
        //if a<b
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
        this->append_instr("SWAP f");  //b to regF
        this->append_instr("RESET a");
        this->append_instr("ADD f");
        this->append_instr("SWAP d");
        this->get_value(a);
        this->append_instr("SWAP e");  //a to regE
        this->append_instr("RESET b");
        this->append_instr("INC b");   //one
        this->append_instr("RESET a");
        this->append_instr("ADD e");   //ac=a
        this->append_instr("SUB f");   //ac=a-b
        this->append_instr("JZERO 19");
        this->append_instr("JNEG 17");

        this->append_instr("RESET a");
        this->append_instr("ADD f");   //cpy b to ac
        this->append_instr("SHIFT b"); //b=b<<1;
        this->append_instr("SUB e");   //
        this->append_instr("JPOS 5");  //b<<1 > a
        this->append_instr("SWAP f");  //swap b to ac
        this->append_instr("SHIFT b"); //b<<1
        this->append_instr("SWAP f");  //b from ac to regF
        this->append_instr("JUMP -8"); //b<<1 > a
        this->append_instr("SWAP e");  //a to ac
        this->append_instr("SUB f");   //a=a-b
        this->append_instr("SWAP e");  //a to regE
        this->append_instr("RESET a");
        this->append_instr("ADD d");
        this->append_instr("SWAP f");
        this->append_instr("JUMP -20");

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

