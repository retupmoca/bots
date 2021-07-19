use std::time::Duration;
use std::thread;
use std::env;

use bots::{World, WorldConfig};

fn main() {
    let mut world = World::new(WorldConfig {
        cpus_per_tick: 1,
        ..WorldConfig::default()
    });

    let args: Vec<String> = env::args().collect();
    if args.len() == 3 {
        world.add_bot(&args[1]);
        world.add_bot(&args[2]);
    }
    else {
        println!("Please provide two robots");
        return;
    }

    world.place_bots();

    loop {
        world.tick();
        println!("Bot 1 heading: {}", world.bots[0].tank.heading);
        println!("Bot 1 position: {}:{}", world.bots[0].tank.x, world.bots[0].tank.y);
        println!("Bot 2 position: {}:{}", world.bots[1].tank.x, world.bots[0].tank.y);
        thread::sleep(Duration::from_millis(1000));
    }
}
