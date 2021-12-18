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

void Generator::gen_const(long long c, long long offset) {
    this->append_instr("RESET a");
    long long k = 0;

    bool neg = false;

    if(c >= 1) {
        this->append_instr("INC a");
        k = 1;
    }

    if(c < 0) {
        c = -c;
        neg = true;
    }
    
    bool flag = false;

    while(abs(c-k) > 0) {
        if(abs(c-k) < 14) {
            if(c-k > 0) {
                this->append_instr("INC a");
                k++;
            } else {
                this->append_instr("DEC a");
                k--;
            }
        }
        else {
            this->append_instr("SWAP b");
            this->append_instr("RESET a");
 
            while(abs(abs(c) - (k<<1)) < abs(abs(c) - k)) {
                this->append_instr("INC a");
                k = k << 1;
            }
            if(c<0)
                k=-k;

            this->append_instr("SWAP b");
            this->append_instr("SHIFT b");

            if(flag) {
                if(c>0) {
                    this->append_instr("ADD c");
                }
                else {
                    this->append_instr("SWAP c");
                    this->append_instr("SUB c");
                }
            }
            if(abs(c-k) >= 20) {
                c-=k;
                k=1;
                this->append_instr("SWAP c");
                this->append_instr("RESET a");
                this->append_instr("INC a");
                flag = true;
            }
            else {
                while(abs(c-k) > 0) {
                    if(c-k > 0) {
                        this->append_instr("INC a");
                        k++;
                    }
                    else {
                        this->append_instr("DEC a");
                        k--;
                    }
                }
            }
        }
    }

    if(neg) {
        this->append_instr("SWAP b");
        this->append_instr("RESET a");
        this->append_instr("SUB b");
    }

    this->append_instr("STORE " + to_string(offset));
}

void Generator::get_value(symbol* sym) {
    if(!sym->initialized) {
        sym->initialized = true;

        if(sym->is_const) {
            this->gen_const(stoll(sym->name), sym->offset);
        }
        else {
            yyerror("variable \e[0;1m‘" + sym->name + "’\e[0m used before being initialized");
        }
    }
    else {
        this->append_instr("LOAD " + to_string(sym->offset));
    }
}

void Generator::gen_assign(symbol* sym) {
    sym->initialized = true;
    this->append_instr("STORE " + to_string(sym->offset));
}

void Generator::gen_read(symbol* sym) {
    this->append_instr("GET");
    this->gen_assign(sym);
}

void Generator::gen_write(symbol* sym) {
    this->get_value(sym);
    this->append_instr("PUT");
}
