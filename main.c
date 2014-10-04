#include <stdio.h>
#include <stdlib.h>

/* CPU ops */

typedef struct s_world world;

typedef struct {
    /* cpu */
    char registers[8];
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
    world* world; /* should be world*, but need to set up predeclarations */
    int machine_id;
} machine;

typedef struct {
    int x;
    int y;
    int heading;
    int speed;
} bot_physics;
struct s_world {
    machine* bots[2];
    bot_physics* botdata[2];
};

void out_reqpos(machine* m) {
    m->ports[1] = m->world->botdata[m->machine_id]->x;
    m->ports[2] = m->world->botdata[m->machine_id]->x;
}

typedef void (*action)(machine*);

action outport_actions[16] = {
    &out_reqpos,
    0
};

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
        (*outport_actions[m->args[0]])(m);
    }
}

void op_in(machine* m) {
    m->registers[m->args[0]] = m->ports[m->args[1]];
}

opdata oplist[6] = {
  { 1, 0, {}, &op_rset },
  { 1, 0, {}, &op_nop },
  { 3, 2, {1, 1}, &op_ldi },
  { 4, 3, {1, 1, 1}, &op_add },
  { 3, 2, {1, 1}, &op_out },
  { 3, 2, {1, 1}, &op_in }
};
/**/

/* bot data */
void machine_execute(machine* m) {
    if(!m->execute_ready)
        return;
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
    world w = { 0 };
    machine m1 = { 0 };
    bot_physics mp1 = { 0 };
    mp1.x = 25;
    mp1.y = 50;

    /* test program */
    /* out 0 0 */
    m1.memory[0] = 4;
    m1.memory[1] = 0;
    m1.memory[2] = 0;

    /* in r3 1 */
    m1.memory[3] = 5;
    m1.memory[4] = 3;
    m1.memory[5] = 1;

    /* in r4 2 */
    m1.memory[6] = 5;
    m1.memory[7] = 4;
    m1.memory[8] = 2;
    /* end test program */

    m1.mem_max=127;
    w.bots[0] = &m1;
    w.botdata[0] = &mp1;
    m1.world = &w;
    while(1) {
        machine_execute(&m1);
        machine_decode(&m1);
        machine_fetch(&m1);
    }
}
