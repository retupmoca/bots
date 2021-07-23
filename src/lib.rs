#[macro_use]
extern crate bitflags;

mod world;
mod cpu;
mod ops;
mod peripherals;

#[cfg(feature = "c_ffi")]
pub mod c_interface;

#[cfg(feature = "unstable_wasm_ffi")]
pub mod wasm_interface;

pub use self::world::{World, WorldConfig};
