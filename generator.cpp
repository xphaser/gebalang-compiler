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

void Generator::gen_const(long long c) {
    this->code.push_back("RESET a");
    long long k = 0;

    bool neg = false;

    if(c >= 1) {
        this->code.push_back("INC a");
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
                this->code.push_back("INC a");
                k++;
            } else {
                this->code.push_back("DEC a");
                k--;
            }
        }
        else {
            this->code.push_back("SWAP b");
            this->code.push_back("RESET a");
 
            while(abs(abs(c) - (k<<1)) < abs(abs(c) - k)) {
                this->code.push_back("INC a");
                k = k << 1;
            }
            if(c<0)
                k=-k;

            this->code.push_back("SWAP b");
            this->code.push_back("SHIFT b");

            if(flag) {
                if(c>0) {
                    this->code.push_back("ADD c");
                }
                else {
                    this->code.push_back("SWAP c");
                    this->code.push_back("SUB c");
                }
            }
            if(abs(c-k) >= 20) {
                c-=k;
                k=1;
                this->code.push_back("SWAP c");
                this->code.push_back("RESET a");
                this->code.push_back("INC a");
                flag = true;
            }
            else {
                while(abs(c-k) > 0) {
                    if(c-k > 0) {
                        this->code.push_back("INC a");
                        k++;
                    }
                    else {
                        this->code.push_back("DEC a");
                        k--;
                    }
                }
            }
        }
    }

    if(neg) {
        this->code.push_back("SWAP b");
        this->code.push_back("RESET a");
        this->code.push_back("SUB b");
    }
}