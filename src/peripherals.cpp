#include <stdlib.h>
#include <math.h>
#include <bots/world.hpp>
#include <bots/peripherals.hpp>
#include <bots/world.hpp>

void bots_peripheral_reset(bots_peripheral *p, bots::World *w, uint8_t id, uint8_t pre) {
    bots_cpu *cpu = w->bots[id]->cpu;
    if(cpu->memory[p->mem_base]) {
        cpu->fetch_pc = 0;
        cpu->fetch_flag = 0;
        cpu->decode_flag = 0;
        cpu->memory[p->mem_base] = 0;
    }
}

void bots_peripheral_radar(bots_peripheral *p, bots::World *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t result_offset = b;
    uint16_t result_range = result_offset + 2;
    uint16_t keepshift = result_range + 2;
    uint16_t arc = keepshift + 1;
    uint16_t range = arc + 1;
    uint16_t scan = range + 2;
    uint16_t steering = scan + 1;
    uint16_t target_offset = steering + 2;

    bots_cpu *cpu = w->bots[i]->cpu;
    bots::Tank &tank = *w->bots[i]->tank;

    if(pre) {
        /* load updated steering offset into io ports */
        cpu->memory[steering] = tank._req_scanner_steering >> 8;
        cpu->memory[steering + 1] = tank._req_scanner_steering & 0xff;
        uint16_t s_target_offset = (tank.scanner_offset + tank._req_scanner_steering) % 1024;
        cpu->memory[target_offset] = s_target_offset >> 8;
        cpu->memory[target_offset + 1] = s_target_offset & 0xff;

        /* done scanning */
        cpu->memory[scan] = 0;
        return;
    }

    /* request scanner steering */
    tank._req_scanner_keepshift = cpu->memory[keepshift];
    uint16_t scanner_steering = cpu->memory[steering] << 8;
    scanner_steering = scanner_steering | cpu->memory[steering+1];
    scanner_steering = scanner_steering % 1024;
    if(scanner_steering == tank._req_scanner_steering) {
        /* user didn't change steering directly; try the target_offset */
        uint16_t s_target_offset = cpu->memory[target_offset] << 8;
        s_target_offset |= cpu->memory[target_offset + 1];

        scanner_steering = (s_target_offset - tank.scanner_offset) % 1024;
    }
    tank._req_scanner_steering = scanner_steering;

    /* do scan */
    if(cpu->memory[scan]) {
        /* get global heading of scanner */
        uint32_t heading = tank.heading + tank.scanner_offset;
        
        /* check angle and range of each bot against scan parameters */
        int radar_arc = cpu->memory[arc];
        int radar_range = cpu->memory[range] << 8;
        radar_range |= cpu->memory[range+1];
        
        uint16_t radar_left = (heading - radar_arc) % 1024;
        uint16_t radar_right = (heading + radar_arc) % 1024;
        
        int seen_index = 0;
        int seen_bots[256];
        int32_t seen_bot_range[256];
        int seen_bot_angle[256];
        
        int j;
        for(j=0; j<w->bots.size(); j++){
            if(j == i)
                continue;
            if(w->bots[j]->tank->health <= 0)
                continue;
            
            int32_t x = w->bots[j]->tank->x - tank.x;
            int32_t y = w->bots[j]->tank->y - tank.y;
            
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
        cpu->memory[result_range] = 0;
        cpu->memory[result_range+1] = 0;
        cpu->memory[result_offset] = 0;
        cpu->memory[result_offset+1] = 0;
        for(j=0; j < seen_index; j++) {
            if(seen_bot_range[j] < lowest_range || lowest_range == 0) {
                lowest_range = seen_bot_range[j];
                cpu->memory[result_range] = lowest_range >> 8;
                cpu->memory[result_range+1] = lowest_range & 0xff;
                
                /* TODO: give some kind of scanner offset instead of bearing */
                cpu->memory[result_offset] = seen_bot_angle[j] >> 8;
                cpu->memory[result_offset+1] = seen_bot_angle[j] & 0xff;
            }
        }
        w->add_event(bots::World::Event::Type::scan, i);
    }
}

void bots_peripheral_turret(bots_peripheral *p, bots::World *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t fire = b;
    uint16_t keepshift = fire + 1;
    uint16_t steering = keepshift + 1;
    uint16_t target_offset = steering + 2;

    bots_cpu *cpu = w->bots[i]->cpu;
    bots::Tank &tank = *w->bots[i]->tank;

    if(pre) {
        cpu->memory[steering] = tank._req_turret_steering >> 8;
        cpu->memory[steering+1] = tank._req_turret_steering & 0xff;
        uint16_t t_target_offset = (tank.turret_offset + tank._req_turret_steering) % 1024;
        cpu->memory[target_offset] = t_target_offset >> 8;
        cpu->memory[target_offset + 1] = t_target_offset & 0xff;

        /* fire */
        cpu->memory[fire] = 0;
        return;
    }

    uint16_t turret_steering = cpu->memory[steering] << 8;
    turret_steering = turret_steering | cpu->memory[steering+1];
    turret_steering = turret_steering % 1024;
    if(turret_steering == tank._req_turret_steering) {
        /* user didn't change steering directly; try the target_offset */
        uint16_t t_target_offset = cpu->memory[target_offset] << 8;
        t_target_offset |= cpu->memory[target_offset + 1];

        turret_steering = (t_target_offset - tank.turret_offset) % 1024;
    }
    tank._req_turret_steering = turret_steering;
    tank._req_turret_keepshift = cpu->memory[keepshift];

    /* fire */
    if(cpu->memory[fire]) {
        bots::Shot s{
            .from = *w->bots[i]
        };
        
        /* get global heading of gun */
        s.heading = tank.heading + tank.turret_offset;
        
        /* move just far enough that we don't hit ourselves */
        s.x = tank.x;
        s.y = tank.y;
            
        double rangle = s.heading * M_PI / 1024;
        int dist = 60;
        int dy = floor(0.5 + (dist * cos(rangle)));
        int dx = floor(0.5 + (dist * sin(rangle)));
        s.x += dx;
        s.y += dy;

        s.id = w->next_shot_id++;
        
        /* add shot to world */
        w->shots.push_back(s);
        w->add_event(bots::World::Event::Type::fire, i);
    }
}

void bots_peripheral_hull(bots_peripheral *p, bots::World *w, uint8_t i, uint8_t pre) {
    uint16_t b = p->mem_base;
    uint16_t m_throttle = b;
    uint16_t m_steering = m_throttle + 1;

    bots_cpu *cpu = w->bots[i]->cpu;
    bots::Tank &tank = *w->bots[i]->tank;

    if(pre) {
        cpu->memory[m_steering] = tank._req_steering >> 8;
        cpu->memory[m_steering+1] = tank._req_steering & 0xff;

        return;
    }

    int8_t throttle = cpu->memory[m_throttle];
    tank._req_throttle = throttle;

    uint16_t steering = cpu->memory[m_steering] << 8;
    steering = steering | cpu->memory[m_steering+1];
    steering = steering % 1024;
    tank._req_steering = steering;
}
