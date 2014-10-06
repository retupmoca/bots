#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

void write_inst(FILE* f, inst* in) {
    int i = 0;
    for(;i<7;i++){
        if(strcmp(mydata[i].name, in->name) == 0){
            putc(i, f);

            int numargs = oplist[i].argcount;
            int j = 0;
            for(;j < numargs; j++){
                char piece = atoi(in->args[j]);
                putc(piece, f);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        return 1;
    }
    char *code;

    FILE* fin = fopen(argv[1], "r");
    fseek(fin, 0L, SEEK_END);
    long s = ftell(fin);
    rewind(fin);
    code = malloc(s+1);
    code[s] = 0;
    fread(code, s, 1, fin);
    fclose(fin);

    FILE* out = fopen(argv[2], "w");

    int i = 0;
    int start = 0;
    inst in;
    int argcount = 0;
    int numargs = 0;
    for(;i == 0 || code[i-1];i++){
        if(code[i] == ' ' || code[i] == '\n' || code[i] == '\0'){
            if(start == i) {
                start = i+1;
            } else {
                int size = i - start;
                char* str = malloc(1 + size);
                str[size] = 0;
                memcpy(str, code + start, size);

                if(argcount == 0) {
                    in.name = str;
                    int i = 0;
                    char found = 0;
                    for(;i<7;i++){
                        if(strcmp(mydata[i].name, in.name) == 0){
                            numargs = oplist[i].argcount;
                            found = 1;
                        }
                    }
                    if(!found){
                        printf("Bad opcode: %s\n", in.name);
                        return 2;
                    }
                }
                else
                    in.args[argcount-1] = str;

                argcount++;

                if(argcount > numargs) { /* todo: get real argcount */
                    printf("Writing instruction %s...\n", in.name);
                    write_inst(out, &in);
                    free(in.name);
                    int j = 1;
                    for(;j<argcount;j++){
                        free(in.args[j-1]);
                    }
                    argcount = 0;
                    numargs = 0;
                }
                start = i+1;
            }
        }
    }

    free(code);
    fclose(out);

    return 0;
}

