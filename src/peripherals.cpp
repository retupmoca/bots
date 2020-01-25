#include <cstdlib>
#include <cmath>
#include <bots/world.hpp>
#include <bots/peripherals.hpp>

namespace bots {
    void ResetPeripheral::write_byte(uint16_t address, uint8_t value) {
        if(value) {
            bot.cpu->fetch_pc = 0;
            bot.cpu->fetch_flag = 0;
            bot.cpu->decode_flag = 0;
        }
    }
}

namespace bots {
    uint8_t RadarPeripheral::read_byte(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::keepshift:
                return bot.tank->_req_scanner_keepshift;
            case (uint16_t)Mem::arc:
                return arc;
            case (uint16_t)Mem::scan:
                return scan;
        }
        return 0;
    }

    uint16_t RadarPeripheral::read_word(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::result_offset:
                return result_offset;
            case (uint16_t)Mem::result_range:
                return result_range;
            case (uint16_t)Mem::range:
                return range;
            case (uint16_t)Mem::steering:
                return bot.tank->_req_scanner_steering;
            case (uint16_t)Mem::target_offset:
                return (bot.tank->scanner_offset + bot.tank->_req_scanner_steering) % 1024;
        }
        return 0;
    }

    void RadarPeripheral::write_byte(uint16_t address, uint8_t value) {
        switch (address) {
            case (uint16_t)Mem::scan:
                scan = value;
                break;
            case (uint16_t)Mem::keepshift:
                bot.tank->_req_scanner_keepshift = value;
                break;
        }
    }

    void RadarPeripheral::write_word(uint16_t address, uint16_t value) {
        switch (address) {
            case (uint16_t)Mem::arc:
                arc = value;
                break;
            case (uint16_t)Mem::range:
                range = value;
                break;
            case (uint16_t)Mem::steering:
                bot.tank->_req_scanner_steering = value;
                break;
            case (uint16_t)Mem::target_offset:
                bot.tank->_req_scanner_steering = value - bot.tank->scanner_offset;
                break;
        }
    }

    void RadarPeripheral::tick() {
        bots::Tank &tank = *bot.tank;

        if (scan) {
            scan = 0;
            /* do scan */
            /* get global heading of scanner */
            uint32_t heading = tank.heading + tank.scanner_offset;
            
            /* check angle and range of each bot against scan parameters */
            uint16_t radar_left = (heading - arc) % 1024;
            uint16_t radar_right = (heading + arc) % 1024;
            
            int seen_index = 0;
            int seen_bots[256];
            int32_t seen_bot_range[256];
            int seen_bot_angle[256];
            
            int j;
            for(j=0; j<bot.world.bots.size(); j++){
                if(bot.world.bots[j].get() == &bot)
                    continue;
                if(bot.world.bots[j]->tank->health <= 0)
                    continue;
                
                int32_t x = bot.world.bots[j]->tank->x - tank.x;
                int32_t y = bot.world.bots[j]->tank->y - tank.y;
                
                uint8_t angle = (int)(atan2(x, y) * 512 / M_PI) % 1024;
                int32_t range = (int)(sqrt(y * y + x * x));
                
                if(   range <= range
                   && (   (radar_left < radar_right && angle > radar_left && angle < radar_right)
                       || (radar_left > radar_right && (angle > radar_left || angle < radar_right)))) {
                    /* we can see bot i */
                    seen_bot_range[seen_index] = range;
                    seen_bot_angle[seen_index] = angle;
                    seen_bots[seen_index++] = j;
                }
            }
            
            /* load closest seen bot into ports */
            result_range = 0;
            result_offset = 0;
            for(j=0; j < seen_index; j++) {
                if(seen_bot_range[j] < result_range || result_range == 0) {
                    result_range = seen_bot_range[j];
                    
                    /* TODO: give some kind of scanner offset instead of bearing */
                    result_offset = seen_bot_angle[j];
                }
            }
            bot.world.add_event(bots::World::Event::Type::scan, bot);
        }
    }
}

namespace bots {
    uint8_t TurretPeripheral::read_byte(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::fire:
                return fire;
            case (uint16_t)Mem::keepshift:
                return bot.tank->_req_turret_keepshift;
        }
        return 0;
    }

    uint16_t TurretPeripheral::read_word(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::steering:
                return bot.tank->_req_turret_steering;
            case (uint16_t)Mem::target_offset:
                return (bot.tank->turret_offset + bot.tank->_req_turret_steering) % 1024;
        }
        return 0;
    }

    void TurretPeripheral::write_byte(uint16_t address, uint8_t value) {
        switch (address) {
            case (uint16_t)Mem::fire:
                fire = value;
                break;
            case (uint16_t)Mem::keepshift:
                bot.tank->_req_turret_keepshift = value;
                break;
        }
    }

    void TurretPeripheral::write_word(uint16_t address, uint16_t value) {
        switch (address) {
            case (uint16_t)Mem::steering:
                bot.tank->_req_turret_steering = value;
                break;
            case (uint16_t)Mem::target_offset:
                bot.tank->_req_turret_steering = value - bot.tank->turret_offset;
                break;
        }
    }

    void TurretPeripheral::tick() {
        bots::Tank &tank = *bot.tank;

        if (fire) {
            fire = 0;
            bots::Shot s{
                .from = bot
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

            s.id = bot.world.next_shot_id++;
            
            /* add shot to world */
            bot.world.shots.push_back(s);
            bot.world.add_event(bots::World::Event::Type::fire, bot);
        }
    }
}

namespace bots {
    uint8_t HullPeripheral::read_byte(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::throttle:
                return bot.tank->_req_throttle;
        }
        return 0;
    }

    uint16_t HullPeripheral::read_word(uint16_t address) {
        switch (address) {
            case (uint16_t)Mem::steering:
                return bot.tank->_req_steering;
        }
        return 0;
    }

    void HullPeripheral::write_byte(uint16_t address, uint8_t value) {
        switch (address) {
            case (uint16_t)Mem::throttle:
                bot.tank->_req_throttle = value;
                break;
        }
    }

    void HullPeripheral::write_word(uint16_t address, uint16_t value) {
        switch (address) {
            case (uint16_t)Mem::steering:
                bot.tank->_req_steering = value;
                break;
        }
    }
}
