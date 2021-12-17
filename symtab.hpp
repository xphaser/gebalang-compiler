#pragma once

#include <string>
#include <map>

using namespace std;

class Symbol {
public:
    long long offset;
    long long value;
    
    Symbol(long long offset) {
        this->offset = offset;
    }
};

class Symtab {
private:
    map<string, Symbol*> symbols;

public:
    long long offset;

    bool find(string pid);
    void putsym(string pid);
    Symbol* getsym(string pid);
    void print();
};
