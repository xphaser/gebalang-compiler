#include "generator.hpp"

long long Generator::get_offset() {
    return this->offset;
}

vector<string> Generator::get_code() {
    return this->code;
}

void Generator::gen_end() {
    this->code.push_back("HALT");
    this->offset++;
}
