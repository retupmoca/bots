#include <stdio.h>
#include <stdlib.h>

/* CPU ops */

typedef struct {
    /* cpu */
    char registers[4];
    short pc;
    short mem_max;
    char memory[128];
    
    char execute_ready;
    char op;
    short args[4];
    
    char decode_ready;
    short decode_pc;
    char decode_bytes[6];

    short fetch_pc;
    /**/

    /* io? */
    char ports[16];

    /* physics? */
    void* world; /* should be world*, but need to set up predeclarations */
    int machine_id;
} machine;

typedef struct {
    int x;
    int y;
    int heading;
    int speed;
} bot_physics;

void out_reqpos(machine* m) {
    /* ... */
}

void* outport_actions[16] = {
    &out_reqpos,
    0
};

typedef struct {
    machine* bots[2];
    bot_physics* botdata[2];
} world;

typedef struct {
    char size;
    char argcount;
    char arg_sizes[3];
    void (*execute)(machine*);
} opdata;

void op_rset(machine* m) {
    m->fetch_pc = 0;
    m->decode_ready = 0;
    m->execute_ready = 0;
}

void op_nop(machine* m) {
    /* nop nop nop */
}

void op_ldi(machine* m) {
    m->registers[m->args[0]] = m->args[1];
}

void op_add(machine* m) {
    m->registers[m->args[0]] = m->registers[m->args[1]] + m->registers[m->args[2]];
}

void op_out(machine* m) {
    m->ports[m->args[0]] = m->args[1];
    if(outport_actions[m->args[0]]){
        /* action(m) */
    }
}

void op_in(machine* m) {
    m->registers[m->args[0]] = m->ports[m->args[1]];
}

opdata oplist[4] = {
  { 1, 0, {}, &op_rset },
  { 1, 0, {}, &op_nop },
  { 3, 2, {1, 1}, &op_ldi },
  { 4, 3, {1, 1, 1}, &op_add }
};
/**/

/* bot data */
void machine_execute(machine* m) {
    (*oplist[m->op].execute)(m);
}

void machine_decode(machine* m) {
    char i;
    if(!m->decode_ready)
        return;
    
    m->op = m->decode_bytes[0];
    for(i = 0; i < oplist[m->op].argcount; i++){
        m->args[i] = m->decode_bytes[1 + i];
    }

    m->execute_ready = 1;
    m->pc = m->decode_pc;
}

void machine_fetch(machine* m) {
    if(m->fetch_pc > m->mem_max) {
        m->fetch_pc = 0;
    }
    char op = m->memory[m->fetch_pc];
    char i;
    for(i=0; i < oplist[op].size; i++){
        if(m->fetch_pc + i <= m->mem_max){
	    m->decode_bytes[i] = m->memory[m->fetch_pc + i];
        }
    }
    m->decode_pc = m->fetch_pc;
    m->decode_ready = 1;
    m->fetch_pc += oplist[op].size;
}
/**/

void main() {
    machine m = { 0 };
    m.mem_max=127;
    while(1) {
        machine_execute(&m);
        machine_decode(&m);
        machine_fetch(&m);
    }    
}
