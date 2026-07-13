use std::path::Path;

use clap::Parser;

use config::CONFIG_PATH;

#[derive(Parser)]
#[command(name = "Sysgrab", bin_name = "sysgrab", version, about)]
struct Args {}

fn main() {
    let _args = Args::parse();
    config::load_or_create_config(Path::new(CONFIG_PATH));
}
