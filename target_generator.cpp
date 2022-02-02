#include <iostream>
#include "target_generator.hpp"
#include "intermediate_generator.hpp"

extern int yyerror(string s);

long long target_generator::get_offset() {
    return this->offset;
}

void target_generator::generate_code(vector<instruction*> instructions) {
    for(int i=0; i<instructions.size(); i++) {
        switch(instructions[i]->name) {
            case ADD: {
                binary* instr = (binary*)instructions[i];
                gen_add(instr->a, instr->b);
                break;
            }
            case SUB: {
                binary* instr = (binary*)instructions[i];
                gen_sub(instr->a, instr->b);
                break;
            }
            case MUL: {
                binary* instr = (binary*)instructions[i];
                gen_mul(instr->a, instr->b);
                break;
            }
            case DIV: {
                binary* instr = (binary*)instructions[i];
                gen_div(instr->a, instr->b);
                break;
            }
            case MOD: {
                binary* instr = (binary*)instructions[i];
                gen_mod(instr->a, instr->b);
                break;
            }
            case STORE: {
                unary* instr = (unary*)instructions[i];
                gen_assign(instr->sym);
            }
                break;
            case LOAD: {
                unary* instr = (unary*)instructions[i];
                get_value(instr->sym);
                break;
            }
            case JUMP: {
                go_to* instr = (go_to*)instructions[i];
                gen_jump(instr->lbl);
                break;
            }
            case JZERO: {
                go_to* instr = (go_to*)instructions[i];
                gen_jzero(instr->lbl);
                break;
            }
            case JNZ: {
                go_to* instr = (go_to*)instructions[i];
                gen_jnz(instr->lbl);
                break;
            }
            case JPOS: {
                go_to* instr = (go_to*)instructions[i];
                gen_jpos(instr->lbl);
                break;
            }
            case JNEG: {
                go_to* instr = (go_to*)instructions[i];
                gen_jneg(instr->lbl);
                break;
            }
            case JNPOS: {
                go_to* instr = (go_to*)instructions[i];
                gen_jnpos(instr->lbl);
                break;
            }
            case JNNEG: {
                go_to* instr = (go_to*)instructions[i];
                gen_jnneg(instr->lbl);
                break;
            }
            case LABEL: {
                go_to* instr = (go_to*)instructions[i];
                if(instr->lbl->address != -1) {
                    backpatch(instr->lbl->address, this->offset - instr->lbl->address);
                }
                else {
                    instr->lbl->address = this->offset;
                }
                break;
            }
            case GET:
                gen_get();
                break;
            case PUT:
                gen_put();
                break;
            case HALT:
                gen_end();
        }
    }
}

vector<string> target_generator::get_code() {
    return this->code;
}

void target_generator::backpatch(long long address, long long value) {
    this->code[address] += to_string(value);
}

void target_generator::append_instr(string instr) {
    this->code.push_back(instr);
    this->offset++;
}

void target_generator::gen_end() {
    this->append_instr("HALT");
}

void target_generator::gen_const(long long c) {
    if(c==0) {
        this->append_instr("RESET a");
        return;
    }
    if(c>0 && c<=10) {
        this->append_instr("RESET a");
        for(int i=0; i<c; i++) {
            this->append_instr("INC a");
        }
        return;
    }
    if(c<0 && c>=-10) {
        this->append_instr("RESET a");
        for(int i=0; i>c; i--) {
            this->append_instr("DEC a");
        }
        return;
    }

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

void target_generator::get_value(symbol* sym) {
    if(sym->is_iterator) {
        sym->initialized = true;
    }
    if(sym->relation >= 0) {
        this->gen_const(sym->relation);
        this->append_instr("LOAD a");
        this->append_instr("SWAP c");
        this->gen_const(sym->offset - sym->array_start);
        this->append_instr("ADD c");
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

void target_generator::gen_address(long long addr) {
    if(addr == 0) {
        this->append_instr("RESET d");
        return;
    }
    else if(addr <= 10) {
        this->append_instr("RESET d");
        for(int i=0; i<addr; i++) {
            this->append_instr("INC d");
        }
        return;
    }
    this->append_instr("SWAP d");
    this->gen_const(addr);
    this->append_instr("SWAP d");
}

void target_generator::gen_assign(symbol* sym) {
    sym->initialized = true;

    if(sym->relation >= 0) {
        this->append_instr("SWAP d");
        this->gen_const(sym->relation);
        this->append_instr("LOAD a");
        this->append_instr("SWAP c");
        this->gen_const(sym->offset - sym->array_start);
        this->append_instr("ADD c");
        this->append_instr("SWAP d");
    }
    else {
        this->gen_address(sym->offset);
    }
    this->append_instr("STORE d");
}

void target_generator::gen_get() {
    this->append_instr("GET");
}

void target_generator::gen_put() {
    this->append_instr("PUT");
}

void target_generator::gen_add(symbol* a, symbol* b) {
    if(b->is_const) {
        long long val = stoll(b->name);
        this->get_value(a);
        if(val <= 10 && val > 0) {
            for(int i=0; i<val; i++) {
                this->append_instr("INC a");
            }
        }
        else if(val >= -10 && val < 0) {
            for(int i=0; i>val; i--) {
                this->append_instr("DEC a");
            }
        }
        else if(val==0) {
            return;
        }
        else {
            this->append_instr("SWAP e"); 
            this->get_value(b);
            this->append_instr("ADD e");         
        }
    }
    else if(a->is_const) {
        long long val = stoll(a->name);
        this->get_value(b);
        if(val <= 10 && val > 0) {
            for(int i=0; i<val; i++) {
                this->append_instr("INC a");
            }
        }
        else if(val >= -10 && val < 0) {
            for(int i=0; i>val; i--) {
                this->append_instr("DEC a");
            }
        }
        else if(val==0) {
            return;
        }
        else {
            this->append_instr("SWAP e"); 
            this->get_value(a);
            this->append_instr("ADD e");         
        }
    }
    else {
        this->get_value(b);
        this->append_instr("SWAP e");
        this->get_value(a);
        this->append_instr("ADD e");
    }
}

void target_generator::gen_sub(symbol* a, symbol* b) {
    if(b->is_const) {
        long long val = stoll(b->name);
        this->get_value(a);
        if(val <= 10 && val > 0) {
            for(int i=0; i<val; i++) {
                this->append_instr("DEC a");
            }
        }
        else if(val >= -10 && val < 0) {
            for(int i=0; i>val; i--) {
                this->append_instr("INC a");
            }
        }
        else if(val==0) {
            return;
        }
        else {
            this->append_instr("SWAP e"); 
            this->get_value(b);
            this->append_instr("SUB e");         
        }
    }
    else if(a->is_const) {
        long long val = stoll(a->name);
        this->get_value(b);
        if(val <= 10 && val > 0) {
            for(int i=0; i<val; i++) {
                this->append_instr("DEC a");
            }
        }
        else if(val >= -10 && val < 0) {
            for(int i=0; i>val; i--) {
                this->append_instr("INC a");
            }
        }
        else if(val==0) {
            return;
        }
        else {
            this->append_instr("SWAP e"); 
            this->get_value(a);
            this->append_instr("SUB e");         
        }
    }
    else {
        this->get_value(b);
        this->append_instr("SWAP e");
        this->get_value(a);
        this->append_instr("SUB e");
    }
}

void target_generator::gen_mul(symbol* a, symbol* b) {
    if(a->is_const) {
        long long val = stoll(a->name);
         if(val > 1 && (val & (val-1)) == 0) {
            int shifts = 0;
            while(val/2 > 0) {
                shifts++;
                val /= 2;
            }
            this->gen_const(shifts);
            this->append_instr("SWAP c");
            this->get_value(b);
            this->append_instr("SHIFT c");
            return;
        }
    }
    if(b->is_const) {
        long long val = stoll(b->name);
         if(val > 1 && (val & (val-1)) == 0) {
            int shifts = 0;
            while(val/2 > 0) {
                shifts++;
                val /= 2;
            }
            this->gen_const(shifts);
            this->append_instr("SWAP c");
            this->get_value(a);
            this->append_instr("SHIFT c");
            return;
        }
    }

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
    this->append_instr("INC e");
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

void target_generator::gen_div(symbol* a, symbol* b) {
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

void target_generator::gen_mod(symbol* a, symbol* b) {
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

void target_generator::gen_jump(goto_label* lbl) {
    if(lbl->address != -1) {
        append_instr("JUMP " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JUMP ");
    }
}

void target_generator::gen_jzero(goto_label* lbl) {
    if(lbl->address != -1) {
        append_instr("JZERO " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JZERO ");
    }
}

void target_generator::gen_jnz(goto_label* lbl) {
    append_instr("JZERO 2");

    if(lbl->address != -1) {
        append_instr("JUMP " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JUMP ");
    }
}

void target_generator::gen_jpos(goto_label* lbl) {
    if(lbl->address != -1) {
        append_instr("JPOS " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JPOS ");
    }
}

void target_generator::gen_jneg(goto_label* lbl) {
    if(lbl->address != -1) {
        append_instr("JNEG " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JNEG ");
    }
}

void target_generator::gen_jnpos(goto_label* lbl) {
    append_instr("JPOS 2");

    if(lbl->address != -1) {
        append_instr("JUMP " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JUMP ");
    }
}

void target_generator::gen_jnneg(goto_label* lbl) {
    append_instr("JNEG 2");

    if(lbl->address != -1) {
        append_instr("JUMP " + to_string(lbl->address - this->offset));
    }
    else {
        lbl->address = this->offset;
        append_instr("JUMP ");
    }
}
