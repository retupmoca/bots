#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bots/ops.h>

typedef struct {
    char* name;
} asm_opdata;

asm_opdata mydata[BOTS_CPU_OPCOUNT] = {
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
    { "sw.rrr" },
    { "sb.rrr" },
    { "sw.rri" },
    { "sb.rri" },
    { "lw.rrr" },
    { "lb.rrr" },
    { "lw.rir" },
    { "lb.rir" },
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
    { "jnz.i" },
    { "call.r" },
    { "call.i" },
    { "ret" }
};

typedef struct {
    char* name;
    char* args[3];
    int argcount;
} inst;

typedef struct {
    char* name;
    int address;
} label_pos;

void write_inst(FILE* f, inst* in) {
    int i = 0;
    for(;i<BOTS_CPU_OPCOUNT;i++){
        if(strcmp(mydata[i].name, in->name) == 0){
            putc(i, f);

            int numargs = bots_cpu_oplist[i].argcount;
            int j = 0;
            for(;j < numargs; j++){
                int32_t piece = atoi(in->args[j]);
                if(bots_cpu_oplist[i].arg_sizes[j] == 2) {
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
    s = fread(code, 1, s, fin);
    code[s] = 0;
    fclose(fin);

    /* pre-process for includes */
    int i = 0;
    int include_start = 0;
    while(include_start > -1) {
        include_start = -1;
        for(;code[i]; i++){
            if(code[i] == '#' && (i == 0 || code[i-1] == '\n')){
                include_start = i;
            }
            if(include_start > -1 && code[i] == '\n'){
                int j = include_start;
                char fname[255];
                char reading = 0;
                for(;j < i; j++){
                    if(!reading && code[j] == ' ') {
                        reading = 1;
                        continue;
                    }

                    if(reading){
                        fname[reading - 1] = code[j];
                        fname[reading] = 0;
                        reading++;
                    }
                }
                char *include;
                FILE *finclude = fopen(fname, "r");
                fseek(finclude, 0L, SEEK_END);
                long size = ftell(finclude);
                rewind(finclude);
                include = malloc(size+1);
                size = fread(include, 1, size, finclude);
                include[size] = 0;
                fclose(finclude);

                char *newcode = malloc(size + s + 1);
                memcpy(newcode, code, include_start);
                memcpy(newcode + include_start, include, size);
                memcpy(newcode + include_start + size, code + i, s - i);
                s = include_start + size + (s - i);
                newcode[s] = 0;

                free(include);
                free(code);
                code = newcode;

                break;
            }
        }
    }

    FILE* out = fopen(argv[2], "wb");

    int start = 0;
    inst in;
    inst program[2048]; 
    int pcount = 0;
    label_pos labels[1024];
    int lcount = 0;
    int argcount = 0;
    int numargs = 0;
    int address = 0;
    int in_comment = 0;
    for(i = 0;i == 0 || code[i-1];i++){
        if(in_comment) {
            in_comment = 1;
            if(code[i] == '\n' || code[i] == '\0'){
                start = i+1;
                in_comment = 0;
            }
        }
        else if(code[i] == ';' || code[i] == ' ' || code[i] == '\n' || code[i] == '\0'){
            if(code[i] == ';'){
                in_comment = 1;
            }
            if(start == i) {
                start = i+1;
            } else {
                int size = i - start;
                char* str = malloc(1 + size);
                str[size] = 0;
                memcpy(str, code + start, size);

                if(argcount == 0) {
                    if(str[0] == '!') {
                        printf("Found label %s (%d)\n", str, address);
                        /* label definition */
                        labels[lcount].name = str;
                        labels[lcount++].address = address;
                        argcount--;
                    }
                    else {
                        in.name = str;
                        int i = 0;
                        char found = 0;
                        for(;i<BOTS_CPU_OPCOUNT;i++){
                            if(strcmp(mydata[i].name, in.name) == 0){
                                numargs = bots_cpu_oplist[i].argcount;
                                address += bots_cpu_oplist[i].size;
                                found = 1;
                            }
                        }
                        if(!found){
                            printf("Bad opcode: %s\n", in.name);
                            return 2;
                        }
                    }
                }
                else {
                    in.args[argcount-1] = str;
                }

                argcount++;

                if(argcount > numargs) {
                    in.argcount = argcount;
                    program[pcount++] = in;
                    argcount = 0;
                    numargs = 0;
                }
                start = i+1;
            }
        }
    }

    free(code);

    for(i = 0; i<pcount; i++){
        int j = 1;
        for(;j<program[i].argcount;j++){
            if(program[i].args[j-1][0] == '!'){
                /* we have a label access! */
                int k = 0;
                int index = -1;
                for(;k < lcount; k++){
                    if(strcmp(labels[k].name, program[i].args[j-1]) == 0){
                        index = k;
                    }
                }
                if(index < 0){
                    printf("Unable to find label %s\n", program[i].args[j-1]);
                    return 2;
                }
                char* newarg = calloc(256, 1);
                sprintf(newarg, "%i", labels[index].address);
                free(program[i].args[j-1]);
                printf("Translating label %s to %s...\n", labels[index].name, newarg);
                program[i].args[j-1] = newarg;
            }
        }
    }

    for(i = 0; i<pcount; i++){
        printf("Writing instruction %s...\n", program[i].name);
        write_inst(out, &program[i]);
        free(program[i].name);
        int j = 1;
        for(;j<program[i].argcount;j++){
            free(program[i].args[j-1]);
        }
    }

    fclose(out);

    return 0;
}

