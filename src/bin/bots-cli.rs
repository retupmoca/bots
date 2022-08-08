use std::time::Duration;
use std::thread;
use std::env;
use std::path::Path;

use bots::{World, WorldConfig};

fn main() {
    let mut world = World::new(WorldConfig {
        cpus_per_tick: 1,
        ..WorldConfig::default()
    });

    let args: Vec<String> = env::args().collect();
    if args.len() == 3 {
        world.add_bot(Path::new(&args[1]));
        world.add_bot(Path::new(&args[2]));
    }
    else {
        println!("Please provide two robots");
        return;
    }

    world.place_bots();

    loop {
        let start = std::time::Instant::now();
        world.tick();
        println!("Tick took: {:?}", start.elapsed());
        println!("{:?}", world.events);
        let tank = world.bots[0].tank_mut();
        println!("Bot 1 heading: {}", tank.heading);
        println!("Bot 1 position: {}:{}", tank.x, tank.y);
        let tank = world.bots[1].tank_mut();
        println!("Bot 2 position: {}:{}", tank.x, tank.y);
        thread::sleep(Duration::from_millis(250));
    }
}
