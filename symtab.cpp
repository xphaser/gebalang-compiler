#include <iostream>
#include "symtab.hpp"

extern int yyerror(string s);

using namespace std;

bool symtab::find(string pid) {
    return this->symbols.find(pid) != this->symbols.end();
}

void symtab::putsym(string pid) {
    if(this->find(pid)) {
        yyerror("redeclaration of \e[0;1m‘" + pid + "’\e[0m");
    }
    else {
        this->symbols[pid] = new symbol(pid, this->offset++, false);
    }
}

void symtab::putarr(string pid, long long a, long long b) {
    if(this->find(pid)) {
        yyerror("redeclaration of \e[0;1m‘" + pid + "’\e[0m");
    }
    else {
        if(b<a) {
            yyerror("invalid \e[0;1m‘" + pid + "’\e[0m array range");
        }
        else {
            this->symbols[pid] = new symbol(pid, this->offset, a, b);
            this->offset += b-a+1;
        }
    }
}

void symtab::putit(string pid) {
    if(this->find(pid)) {
        yyerror("redeclaration of \e[0;1m‘" + pid + "’\e[0m");
    }
    else {
        this->symbols[pid] = new symbol(pid, this->offset++, true);
    }
}

symbol* symtab::getsym(string pid) {
    return this->symbols[pid];
}

void symtab::delsym(string pid) {
    symbols.erase(pid);
}

symbol* symtab::get_const(long long val) {
    string pid = to_string(val);

    //if(this->find(pid)) {
    //    return this->getsym(pid);
    //}
    //else {
    //    this->putsym(pid);
        symbol* sym =  new symbol(pid, -1, false);
        sym->is_const = true;
        return sym;
    
}

symbol* symtab::get_var(string pid) {
    if(this->find(pid)) {
        return this->getsym(pid);
    }
    else {
        yyerror("‘" + pid + "’\e[0m is not defined");
    }
}

symbol* symtab::get_var(string pid, long long i) {
    if(!this->find(pid)) {
        yyerror("‘" + pid + "’\e[0m is not defined");
    }
    else {
        symbol* arr = this->getsym(pid);
        if(!arr->is_array) {
            yyerror("incorrect use of variable \e[0;1m‘" + pid + "’\e[0m");
        }
        else {
            long long offset = arr->offset + i - arr->array_start;
            string* name = new string(pid + "[" + to_string(i) + "]");
            symbol* sym = new symbol(*name, offset, false);
            sym->initialized = true;
            return sym;
        }
    }
}

symbol* symtab::get_var(string pid, string i) {
    if(!this->find(i)) {
        yyerror("‘" + i + "’\e[0m is not defined");
    }
    else if(!this->find(pid)) {
        yyerror("‘" + pid + "’\e[0m is not defined");
    }
    else {
        symbol* arr = this->getsym(pid);
        symbol* var = this->getsym(i);
        //if(!var->initialized && !var->is_iterator) {
        //    yyerror("variable \e[0;1m‘" + var->name + "’\e[0m used before being initialized");
        //}
        //else
        if(!arr->is_array) {
            yyerror("incorrect use of variable \e[0;1m‘" + pid + "’\e[0m");
        }
        else {
            symbol* sym = new symbol(pid, arr->offset, arr->array_start, arr->array_end, var->offset);
            sym->initialized = true;
            return sym;
        }
    }
}

symbol* symtab::get_new_temp() {
    string temp = "t" + to_string(++this->temps);
    putsym(temp);
    return getsym(temp);
}

void symtab::print() {
    map<string, symbol*>::iterator it;

    for (it = this->symbols.begin(); it != this->symbols.end(); it++) {
        cout << "PID: " << it->first << "\t" << "offset: " << it->second->offset << endl;
    }
}
