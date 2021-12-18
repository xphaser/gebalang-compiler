#pragma once

#include <string>
#include <map>

using namespace std;

typedef struct symbol {
    long long offset;
    string name;
    bool is_const = false;
    bool initialized = false;

    symbol(string pid, long long offset) {
        this->name = pid;
        this->offset = offset;
    }
} symbol;

class Symtab {
private:
    map<string, symbol*> symbols;

public:
    long long offset;

    bool find(string pid);
    void putsym(string pid);
    symbol* getsym(string pid);
    symbol* get_const(long long val);
    symbol* get_var(string pid);
    void print();
};
