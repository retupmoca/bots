#ifndef STRUCT_H
#define STRUCT_H

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

#endif
