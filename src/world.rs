use std::path::Path;
use std::fs::File;
use std::io::Read;
use std::f64::consts::PI;

use crate::cpu::Cpu;

pub struct WorldConfig {
    pub cpus_per_tick: u8,
    pub spawn_distance: i32,

    pub hull_turn_rate: u16,
    pub turret_turn_rate: u16,
    pub scanner_turn_rate: u16
}

impl Default for WorldConfig {
    fn default() -> Self {
        WorldConfig {
            cpus_per_tick: 2,
            spawn_distance: 100,

            hull_turn_rate: 1,
            turret_turn_rate: 2,
            scanner_turn_rate: 8,
        }
    }
}

pub struct World {
    config: WorldConfig,
    pub bots: Vec<Bot>,
    shots: Vec<Shot>
}

impl World {
    pub fn new(config: WorldConfig) -> Self {
        World {
            config,
            bots: vec![],
            shots: vec![],
        }
    }

    pub fn add_bot(&mut self, filename: &str) {
        self.bots.push(Bot::from(Path::new(filename)));
    }

    pub fn place_bots(&mut self) {
        let dist = self.config.spawn_distance;

        let mut angle = 0f64;
        let step = (2f64 * PI) / self.bots.len() as f64;

        for bot in &mut self.bots {
            let loc_x = (dist as f64 * angle.cos() + 0.5).floor() as i32;
            let loc_y = (dist as f64 * angle.sin() + 0.5).floor() as i32;
            bot.tank.x = loc_x;
            bot.tank.y = loc_y;
            angle += step;
        }
    }

    pub fn tick(&mut self) {
        // TODO: events?
        
        self.physics_tick();
        self.process_tick();
    }

    fn physics_tick(&mut self) {
        /* run world physics */
        /* shots */
        let mut shots_idx_to_delete: Vec<usize> = Vec::new();
        for (i, shot) in self.shots.iter_mut().enumerate() {
            let mut hit = false;
            for bot in &mut self.bots {
                if shot.x >= bot.tank.x - 40
                && shot.x <= bot.tank.x + 40
                && shot.y >= bot.tank.y - 40
                && shot.y <= bot.tank.y + 40
                && bot.tank.health > 0 {
                    hit = true;
                    //TODO: add_event
                    bot.tank.health -= 10;
                    if bot.tank.health <= 0 {
                        bot.tank.health = 0;
                        // TODO add_event
                    }

                    shots_idx_to_delete.push(i);
                    break;
                }
            }
            if hit { continue; }

            let rangle = shot.heading as f64 * PI / 512f64;
            let dist = 20f64;
            let dy = (dist * rangle.cos() + 0.5).floor() as i32;
            let dx = (dist * rangle.sin() + 0.5).floor() as i32;
            let start_x = shot.x;
            let start_y = shot.y;
            shot.x += dx;
            shot.y += dy;
            if (start_x - shot.x).abs() > dx * 2 || (start_y - shot.y) > dy * 2 {
                // detect wrapping and delete shot
                // TODO: rust probably just blows up here?
                shots_idx_to_delete.push(i);
                continue;
            }

            /* check collision again */
            for bot in &mut self.bots {
                if shot.x >= bot.tank.x - 40
                && shot.x <= bot.tank.x + 40
                && shot.y >= bot.tank.y - 40
                && shot.y <= bot.tank.y + 40
                && bot.tank.health > 0 {
                    hit = true;
                    //TODO: add_event
                    bot.tank.health -= 10;
                    if bot.tank.health <= 0 {
                        bot.tank.health = 0;
                        // TODO add_event
                    }

                    shots_idx_to_delete.push(i);
                    break;
                }
            }
        }
        /* bots */
        for bot in &mut self.bots {
            if bot.tank.health <= 0 {
                continue;
            }

            /* turn, etc */
            let steering = bot.tank._req_steering;
            let mut real_steering = steering;
            if real_steering <= 512 && real_steering > self.config.hull_turn_rate {
                real_steering = self.config.hull_turn_rate;
            }
            if real_steering > 512 && real_steering < (1024 - self.config.hull_turn_rate) {
                real_steering = 1024 - self.config.hull_turn_rate;
            }
            bot.tank._req_steering -= real_steering;

            bot.tank.heading = (bot.tank.heading + real_steering as u32) % 1024;
            bot.tank.speed = bot.tank._req_throttle as i32;
            if bot.tank.speed > 100 {
                bot.tank.speed = 100;
            }

            /* drive! */
            let rangle = bot.tank.heading as f64 * PI / 512f64;
            let dist = bot.tank.speed / 100 * 6;
            let dy = (dist as f64 * rangle.cos() + 0.5).floor() as i32;
            let dx = (dist as f64 * rangle.sin() + 0.5).floor() as i32;
            bot.tank.x += dx;
            bot.tank.y += dy;

            /* turn turret */
            let mut turret_steering = bot.tank._req_turret_steering;
            if bot.tank._req_turret_keepshift != 0 {
                turret_steering = (turret_steering + 1024 - real_steering) % 1024;
            }
            turret_steering = turret_steering % 1024;

            let mut real_turret_steering = turret_steering;
            if real_turret_steering <= 512 && real_turret_steering > self.config.turret_turn_rate {
                real_turret_steering = self.config.turret_turn_rate;
            }
            if real_turret_steering > 512 && real_turret_steering < (1024 - self.config.turret_turn_rate) {
                real_turret_steering = 1024 - self.config.turret_turn_rate;
            }
            bot.tank._req_turret_steering -= real_turret_steering;

            bot.tank.turret_offset = (bot.tank.turret_offset + real_turret_steering as u32) % 1024;

            /* turn scanner */
            let mut scanner_steering = bot.tank._req_scanner_steering;
            if bot.tank._req_scanner_keepshift != 0 {
                scanner_steering = (scanner_steering + 1024 - real_steering) % 1024;
            }
            scanner_steering = scanner_steering % 1024;

            let mut real_scanner_steering = scanner_steering;
            if real_scanner_steering <= 512 && real_scanner_steering > self.config.scanner_turn_rate {
                real_scanner_steering = self.config.scanner_turn_rate;
            }
            if real_scanner_steering > 512 && real_scanner_steering < (1024 - self.config.scanner_turn_rate) {
                real_scanner_steering = 1024 - self.config.scanner_turn_rate;
            }
            bot.tank._req_scanner_steering -= real_scanner_steering;

            bot.tank.scanner_offset = (bot.tank.scanner_offset + real_scanner_steering as u32) % 1024;
        }
    }

    fn process_tick(&mut self) {
        //todo!();
    }
}

struct Shot {
    x: i32,
    y: i32,
    heading: u32,
    bot_id: usize,
}

pub struct Bot {
    pub tank: Tank,
    cpu: Cpu
}

impl From<&Path> for Bot {
    fn from(file: &Path) -> Self {
        let mut file = File::open(file).expect("File not found.");
        let reader: &mut dyn Read = &mut file;
        Self::from(reader)
    }
}

impl From<&mut dyn Read> for Bot {
    fn from(reader: &mut dyn Read) -> Self {
        let mut data: Vec<u8> = vec![];
        reader.read_to_end(&mut data).expect("Read failed");
        Self::from(&data)
    }
}

impl From<&Vec<u8>> for Bot {
    fn from(data: &Vec<u8>) -> Self {
        let mut bot = Bot {
            tank: Tank::default(),
            cpu: Cpu::default()
        };

        bot.tank.health = 100;
        bot.cpu.user_mem_max = 0xefff;
        bot.cpu.registers[10] = 0xefff;
        for (i, elem) in data.iter().enumerate() {
            bot.cpu.memory[i] = *elem;
        }

        // TODO: mount peripherals

        bot
    }
}

#[derive(Default)]
pub struct Tank {
    pub x: i32,
    pub y: i32,

    pub heading: u32,
    speed: i32,
    turret_offset: u32,
    scanner_offset: u32,

    health: u8,

    _req_steering: u16,
    _req_throttle: i16,
    _req_turret_steering: u16,
    _req_turret_keepshift: u8,
    _req_scanner_steering: u16,
    _req_scanner_keepshift: u8
}
