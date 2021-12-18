#include <iostream>
#include "symtab.hpp"

extern int yyerror(string s);

using namespace std;

bool Symtab::find(string pid) {
    map<string, symbol*>::iterator it;
    it = this->symbols.find(pid);
    return (it != this->symbols.end());
}

void Symtab::putsym(string pid) {
    if(this->find(pid)) {
        yyerror("redeclaration of \e[0;1m‘" + pid + "’\e[0m");
    }
    else {
        this->symbols[pid] = new symbol(pid, ++this->offset);
    }
}

symbol* Symtab::getsym(string pid) {
    return this->symbols[pid];
}

symbol* Symtab::get_const(long long val) {
    string pid = to_string(val);

    if(this->find(pid)) {
        return this->getsym(pid);
    }
    else {
        this->putsym(pid);
        symbol* sym = this->getsym(pid);
        sym->is_const = true;
        return sym;
    }
}

symbol* Symtab::get_var(string pid) {
    if(this->find(pid)) {
        return this->getsym(pid);
    }
    else {
        yyerror("‘" + pid + "’\e[0m is not defined");
    }
}

void Symtab::print() {
    map<string, symbol*>::iterator it;

    for (it = this->symbols.begin(); it != this->symbols.end(); it++) {
        cout << "PID: " << it->first << "\t" << "offset: " << it->second->offset << endl;
    }
}
