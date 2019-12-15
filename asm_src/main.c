#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
    char* name;
    char* flags;
    int argcount;
    int opts;
} asm_opdata;

#define OPCOUNT 16
#define OPT_CAN_SWAP 1
#define OPT_START_RB 2

asm_opdata ops[OPCOUNT] = {
    { "nop", "", 0, 0 },
    { "add", "", 3, 0 },
    { "sub", "", 3, OPT_CAN_SWAP },
    { "mul", "", 3, 0 },
    { "div", "  %", 3, OPT_CAN_SWAP },
    { "or", "", 3, 0 },
    { "and", "", 3, 0 },
    { "xor", "", 3, 0 },
    { "shift", "  <", 3, OPT_CAN_SWAP },
    { "not", "", 2, 0 },
    { "push", "", 1, 0 },
    { "pop", "", 1, 0 },
    { "store", " b", 3, OPT_START_RB },
    { "load", " b", 3, 0 },
    { "cmp", "", 2, OPT_START_RB | OPT_CAN_SWAP },
    { "jmp", " zZ<=!>c", 2, OPT_START_RB }
};

typedef struct {
    char* name;
    char* flags;
    char* args[3];
    int argcount;
} inst;

typedef struct {
    char* name;
    int address;
} label_pos;

int write_inst(FILE* f, inst* in) {
    uint8_t op = 0;
    uint8_t flags = 0;
    uint8_t ra = 0;
    uint8_t rb = 0;
    uint16_t imm = 0;

    int i = 0;
    for(;i<OPCOUNT;i++){
        if(strcmp(ops[i].name, in->name) == 0){
            int j = 0;

            op = i;

            if(in->flags) {
                for(int k=0; in->flags[k] != '\0'; k++) {
                    char found = 0;
                    for(j=0; ops[i].flags[j] != '\0'; j++) {
                        if(in->flags[k] == ops[i].flags[j]) {
                            found = 1;
                            if(j < 4)
                                flags |= 0x08 >> j;
                            else
                                ra |= 0x08 >> (j - 4);
                        }
                    }
                    if(!found) {
                        printf("ERR: Unknown flag %c", in->flags[k]);
                        return 2;
                    }
                }
            }

            int numargs = ops[i].argcount;
            char can_swap = ops[i].opts & OPT_CAN_SWAP;
            char start_rb = ops[i].opts & OPT_START_RB;
            char saw_imm = 0;
            for(j=0; j < numargs; j++){
                char need_reg = 0;
                if(j == 0 && !(can_swap && start_rb)) {
                    need_reg = 1;
                }
                if(j == 1 && !(can_swap || start_rb)) {
                    need_reg = 1;
                }

                char is_reg = (in->args[j][0] == 'r');
                if(!is_reg && need_reg) {
                    printf("ERR: Opcode requires argument %d to be a register (r0-r11)",
                           j+1);
                    return 2;
                }
                if(!is_reg)
                    saw_imm++;
                if(saw_imm > 1) {
                    printf("ERR: Opcode can only take a single immediate value");
                    return 2;
                }

                /* set flag for immediate-as-register */
                if(j == 2 && !saw_imm)
                    flags |= 0x08;
                if(j == 1 && start_rb && !saw_imm)
                    flags |= 0x08;
                /* set flag for swapped order of immediate */
                if(j == 1 && !start_rb && can_swap && !is_reg)
                    flags |= 0x04;
                if(j == 0 && start_rb && can_swap && !is_reg)
                    flags |= 0x04;

                int32_t piece;
                if(is_reg)
                    piece = atoi(in->args[j] + 1);
                else if(in->args[j][0] == '0' && in->args[j][1] == 'x')
                    sscanf(in->args[j], "%x", &piece);
                else
                    piece = atoi(in->args[j]);

                if(!is_reg)
                    imm = piece;
                else {
                    if(start_rb) {
                        if(j == 0)
                            rb = piece;
                        if(j == 1 && !saw_imm)
                            imm = piece;
                        if(j == 1 && saw_imm)
                            rb = piece;
                        if(j == 2)
                            ra = piece;
                    }
                    else {
                        if(j == 0)
                            ra = piece;
                        if(j == 1)
                            rb = piece;
                        if(j == 2 && !saw_imm)
                            imm = piece;
                        if(j == 2 && saw_imm)
                            rb = piece;
                    }
                }
            }
        }
    }

    putc((op << 4) | flags, f);
    putc((ra << 4) | rb, f);
    putc(imm >> 8, f);
    putc(imm & 0xff, f);

    return 0;
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
                        in.flags = 0;
                        int j;
                        for(j=0; str[j] != '\0'; j++) {
                            if(str[j] == '.')
                                break;
                        }
                        if(str[j] == '.') {
                            str[j] = '\0';
                            in.flags = str + j + 1;
                        }

                        char found = 0;
                        for(j=0;j<OPCOUNT;j++){
                            if(strcmp(ops[j].name, in.name) == 0){
                                numargs = ops[j].argcount;
                                address += 4;
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
        int ret = write_inst(out, &program[i]);
        if(ret)
            return ret;
        free(program[i].name);
        int j = 1;
        for(;j<program[i].argcount;j++){
            free(program[i].args[j-1]);
        }
    }

    fclose(out);

    return 0;
}

