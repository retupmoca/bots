#include <stdlib.h>
#include <math.h>
#include <bots/world.h>
#include <bots/peripherals.h>

void bots_peripheral_reset(bots_peripheral *p, bots_world *w, uint8_t id, uint8_t pre) {
    if(w->cpus[id]->memory[p->mem_base]) {
        w->cpus[id]->fetch_pc = 0;
        w->cpus[id]->fetch_flag = 0;
        w->cpus[id]->decode_flag = 0;
        w->cpus[id]->memory[p->mem_base] = 0;
    }
}

void bots_peripheral_radar(bots_peripheral *p, bots_world *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t result_offset = b;
    uint16_t result_range = result_offset + 2;
    uint16_t keepshift = result_range + 2;
    uint16_t arc = keepshift + 1;
    uint16_t range = arc + 1;
    uint16_t scan = range + 2;
    uint16_t steering = scan + 1;
    uint16_t target_offset = steering + 2;

    if(pre) {
        /* load updated steering offset into io ports */
        w->cpus[i]->memory[steering] = w->tanks[i]->_req_scanner_steering >> 8;
        w->cpus[i]->memory[steering + 1] = w->tanks[i]->_req_scanner_steering & 0xff;
        uint16_t s_target_offset = (w->tanks[i]->scanner_offset + w->tanks[i]->_req_scanner_steering) % 1024;
        w->cpus[i]->memory[target_offset] = s_target_offset >> 8;
        w->cpus[i]->memory[target_offset + 1] = s_target_offset & 0xff;

        /* done scanning */
        w->cpus[i]->memory[scan] = 0;
        return;
    }

    /* request scanner steering */
    w->tanks[i]->_req_scanner_keepshift = w->cpus[i]->memory[keepshift];
    uint16_t scanner_steering = w->cpus[i]->memory[steering] << 8;
    scanner_steering = scanner_steering | w->cpus[i]->memory[steering+1];
    scanner_steering = scanner_steering % 1024;
    if(scanner_steering == w->tanks[i]->_req_scanner_steering) {
        /* user didn't change steering directly; try the target_offset */
        uint16_t s_target_offset = w->cpus[i]->memory[target_offset] << 8;
        s_target_offset |= w->cpus[i]->memory[target_offset + 1];

        scanner_steering = (s_target_offset - w->tanks[i]->scanner_offset) % 1024;
    }
    w->tanks[i]->_req_scanner_steering = scanner_steering;

    /* do scan */
    if(w->cpus[i]->memory[scan]) {
        /* get global heading of scanner */
        uint32_t heading = w->tanks[i]->heading + w->tanks[i]->scanner_offset;
        
        /* check angle and range of each bot against scan parameters */
        int radar_arc = w->cpus[i]->memory[arc];
        int radar_range = w->cpus[i]->memory[range] << 8;
        radar_range |= w->cpus[i]->memory[range+1];
        
        uint16_t radar_left = (heading - radar_arc) % 1024;
        uint16_t radar_right = (heading + radar_arc) % 1024;
        
        int seen_index = 0;
        int seen_bots[256];
        int32_t seen_bot_range[256];
        int seen_bot_angle[256];
        
        int j;
        for(j=0; j<w->num_tanks; j++){
            if(j == i)
                continue;
            if(w->tanks[j]->health <= 0)
                continue;
            
            int32_t x = w->tanks[j]->x - w->tanks[i]->x;
            int32_t y = w->tanks[j]->y - w->tanks[i]->y;
            
            uint8_t angle = (int)(atan2(x, y) * 512 / M_PI) % 1024;
            int32_t range = (int)(sqrt(y * y + x * x));
            
            if(   range <= radar_range
               && (   (radar_left < radar_right && angle > radar_left && angle < radar_right)
                   || (radar_left > radar_right && (angle > radar_left || angle < radar_right)))) {
                /* we can see bot i */
                seen_bot_range[seen_index] = range;
                seen_bot_angle[seen_index] = angle;
                seen_bots[seen_index++] = j;
            }
        }
        
        /* load closest seen bot into ports */
        uint16_t lowest_range = 0;
        w->cpus[i]->memory[result_range] = 0;
        w->cpus[i]->memory[result_range+1] = 0;
        w->cpus[i]->memory[result_offset] = 0;
        w->cpus[i]->memory[result_offset+1] = 0;
        for(j=0; j < seen_index; j++) {
            if(seen_bot_range[j] < lowest_range || lowest_range == 0) {
                lowest_range = seen_bot_range[j];
                w->cpus[i]->memory[result_range] = lowest_range >> 8;
                w->cpus[i]->memory[result_range+1] = lowest_range & 0xff;
                
                /* TODO: give some kind of scanner offset instead of bearing */
                w->cpus[i]->memory[result_offset] = seen_bot_angle[j] >> 8;
                w->cpus[i]->memory[result_offset+1] = seen_bot_angle[j] & 0xff;
            }
        }
        bots_add_event(w, BOTS_EVENT_SCAN, i);
    }
}

void bots_peripheral_turret(bots_peripheral *p, bots_world *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t fire = b;
    uint16_t keepshift = fire + 1;
    uint16_t steering = keepshift + 1;

    if(pre) {
        w->cpus[i]->memory[steering] = w->tanks[i]->_req_turret_steering >> 8;
        w->cpus[i]->memory[steering+1] = w->tanks[i]->_req_turret_steering & 0xff;

        /* fire */
        w->cpus[i]->memory[fire] = 0;
        return;
    }

    uint16_t turret_steering = w->cpus[i]->memory[steering] << 8;
    turret_steering = turret_steering | w->cpus[i]->memory[steering+1];
    turret_steering = turret_steering % 1024;
    w->tanks[i]->_req_turret_steering = turret_steering;
    w->tanks[i]->_req_turret_keepshift = w->cpus[i]->memory[keepshift];

    /* fire */
    if(w->cpus[i]->memory[fire]) {
        bots_shot* s = malloc(sizeof(bots_shot));
        
        /* get global heading of gun */
        s->heading = w->tanks[i]->heading + w->tanks[i]->turret_offset;
        
        /* move just far enough that we don't hit ourselves */
        s->x = w->tanks[i]->x;
        s->y = w->tanks[i]->y;
            
        double rangle = s->heading * M_PI / 1024;
        int dist = 60;
        int dy = floor(0.5 + (dist * cos(rangle)));
        int dx = floor(0.5 + (dist * sin(rangle)));
        s->x += dx;
        s->y += dy;

        s->bot_id = i;
        s->id = w->next_shot_id++;
        
        /* add shot to world */
        int i = 0;
        for(; w->shots[i]; i++)
            ;
        w->shots[i] = s;
        bots_add_event(w, BOTS_EVENT_FIRE, i);
    }
}

void bots_peripheral_hull(bots_peripheral *p, bots_world *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t m_throttle = b;
    uint16_t m_steering = m_throttle + 1;

    if(pre) {
        w->cpus[i]->memory[m_steering] = w->tanks[i]->_req_steering >> 8;
        w->cpus[i]->memory[m_steering+1] = w->tanks[i]->_req_steering & 0xff;

        return;
    }

    int8_t throttle = w->cpus[i]->memory[m_throttle];
    w->tanks[i]->_req_throttle = throttle;

    uint16_t steering = w->cpus[i]->memory[m_steering] << 8;
    steering = steering | w->cpus[i]->memory[m_steering+1];
    steering = steering % 1024;
    w->tanks[i]->_req_steering = steering;
}
