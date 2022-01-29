#pragma once

#include <string>
#include <map>

using namespace std;

typedef struct symbol {
    long long offset;
    string name;
    bool is_const = false;
    bool is_array = false;
    bool is_iterator = false;
    bool initialized = false;
    long long relation = -1;
    long long array_start;
    long long array_end;

    symbol(string pid, long long offset, bool iterator) {
        this->name = pid;
        this->offset = offset;
        this->is_iterator = iterator;
    }

    symbol(string pid, long long offset, long long a, long long b) {
        this->name = pid;
        this->offset = offset;
        this->is_array = true;
        this->array_start = a;
        this->array_end = b;
    }

    symbol(string pid, long long offset, long long a, long long b, long long c) {
        this->name = pid;
        this->offset = offset;
        this->is_array = true;
        this->relation = c;
        this->array_start = a;
        this->array_end = b;
    }
} symbol;

class symtab {
private:
    map<string, symbol*> symbols;

public:
    long long offset;
    int temps = 0; 

    bool find(string pid);
    void putsym(string pid);
    void putarr(string pid, long long a, long long b);
    void putit(string pid);
    symbol* getsym(string pid);
    void delsym(string pid);
    symbol* get_const(long long val);
    symbol* get_var(string pid);
    symbol* get_var(string pid, long long i);
    symbol* get_var(string pid, string i);
    symbol* get_new_temp();
    void print();
};
