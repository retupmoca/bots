#[macro_use]
extern crate bitflags;

#[cfg(feature="wasm_ffi")]
extern crate wee_alloc;
#[cfg(feature="wasm_ffi")]
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

mod world;
mod cpu;
mod ops;
mod peripherals;

#[cfg(any(feature="c_ffi", feature="wasm_ffi"))]
pub mod ffi_interface;

pub use self::world::{World, WorldConfig};
