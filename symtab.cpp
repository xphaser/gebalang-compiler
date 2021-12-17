#include <iostream>
#include "symtab.hpp"

using namespace std;

bool Symtab::find(string pid) {
    map<string, Symbol*>::iterator it;
    it = this->symbols.find(pid);
    return (it != this->symbols.end());
}

void Symtab::putsym(string pid) {
    if(this->find(pid)) {
        cout << "err" << endl;
    }
    else {
        this->symbols[pid] = new Symbol(++this->offset);
    }
}

Symbol* Symtab::getsym(string pid) {
    return this->symbols[pid];
}

void Symtab::print() {
    map<string, Symbol*>::iterator it;

    for (it = this->symbols.begin(); it != this->symbols.end(); it++) {
        cout << "PID: " << it->first << "\t" << "offset: " << it->second->offset << endl;
    }
}
