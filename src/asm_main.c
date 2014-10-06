#include <stdlib.h>
#include <string.h>
#include "ops.h"

typedef struct {
    char* name;
} asm_opdata;

asm_opdata mydata[7] = {
    { "rset" },
    { "nop" },
    { "mov.i" },
    { "add.rr" },
    { "out.ii" },
    { "in.i" },
    { "eint.i" }
};

typedef struct {
    char* name;
    char* args[3];
} inst;

void write_inst(inst* in) {
    /* todo */
}

void main() {
    char* code = "out.ii 3 128 out.ii 4 5";

    int i = 0;
    int start = 0;
    inst in;
    int argcount = 0;
    for(;i == 0 || code[i-1];i++){
        if(code[i] == ' ' || code[i] == '\n' || code[i] == '\0'){
            if(start == i) {
                start = i+1;
            } else {
                int size = i - start;
                char* str = malloc(1 + size);
                str[size] = 0;
                memcpy(str, code + start, size);

                if(argcount == 0)
                    in.name = str;
                else
                    in.args[argcount-1] = str;

                argcount++;

                if(argcount > 2) { /* todo: get real argcount */
                    write_inst(&in);
                    free(in.name);
                    int j = 1;
                    for(;j<argcount;j++){
                        free(in.args[j-1]);
                    }
                    argcount = 0;
                }
            }
        }
    }
}

