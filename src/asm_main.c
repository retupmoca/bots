#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bots/ops.h>

typedef struct {
    char* name;
} asm_opdata;

asm_opdata mydata[OPCOUNT] = {
    { "nop" },
    { "mov.r" },
    { "mov.i" },
    { "add.rr" },
    { "add.ri" },
    { "sub.rr" },
    { "sub.ri" },
    { "sub.ir" },
    { "mul.rr" },
    { "mul.ri" },
    { "div.rr" },
    { "div.ri" },
    { "div.ir" },
    { "mod.rr" },
    { "mod.ri" },
    { "mod.ir" },
    { "neg" },
    { "or.rr" },
    { "or.ri" },
    { "and.rr" },
    { "and.ri" },
    { "xor.rr" },
    { "xor.ri" },
    { "shl.rr" },
    { "shl.ri" },
    { "shl.ir" },
    { "shr.rr" },
    { "shr.ri" },
    { "shr.ir" },
    { "not" },
    { "push.r" },
    { "push.i" },
    { "pop" },
    { "put.rr" },
    { "put.ri" },
    { "put.ir" },
    { "put.ii" },
    { "get.r" },
    { "get.i" },
    { "out.rr" },
    { "out.ri" },
    { "out.ir" },
    { "out.ii" },
    { "in.r" },
    { "in.i" },
    { "int.r" },
    { "int.i" },
    { "cmp.rr" },
    { "cmp.ri" },
    { "cmp.ir" },
    { "jmp.r" },
    { "jmp.i" },
    { "jls.r" },
    { "jls.i" },
    { "jgr.r" },
    { "jgr.i" },
    { "jne.r" },
    { "jne.i" },
    { "jeq.r" },
    { "jeq.i" },
    { "jge.r" },
    { "jge.i" },
    { "jle.r" },
    { "jle.i" },
    { "jz.r" },
    { "jz.i" },
    { "jnz.r" },
    { "jnz.i" }
};

typedef struct {
    char* name;
    char* args[3];
} inst;

void write_inst(FILE* f, inst* in) {
    int i = 0;
    for(;i<OPCOUNT;i++){
        if(strcmp(mydata[i].name, in->name) == 0){
            putc(i, f);

            int numargs = oplist[i].argcount;
            int j = 0;
            for(;j < numargs; j++){
                int32_t piece = atoi(in->args[j]);
                if(oplist[i].arg_sizes[j] == 2) {
                    putc((char)((piece & 0xff00) >> 8), f);
                    putc((char)(piece & 0x00ff), f);
                } else {
                    putc((char)(piece & 0x00ff), f);
                }
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
                    for(;i<OPCOUNT;i++){
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

                if(argcount > numargs) {
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

