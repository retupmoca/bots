fn main() {
    if cfg!(target_os = "linux") {
        println!("cargo:rustc-cdylib-link-arg=-Wl,-soname,libbots.so");
    }
}
