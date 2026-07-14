use std::io::{self, Write};
use std::path::Path;

use clap::Parser;
use colored::Colorize;

use art::{load_art, Art};
use config::{load_or_create_config, CONFIG_PATH};

#[derive(Parser)]
#[command(name = "Sysgrab", bin_name = "sysgrab", version, about)]
struct Args {}

fn print(art: &Art, accent_color: &[u8; 3], _base_color: &[u8; 3]) {
    let stdout = io::stdout();
    let mut handle = stdout.lock();

    for line in &art.data {
        writeln!(
            handle,
            "{}",
            line.truecolor(accent_color[0], accent_color[1], accent_color[2])
        )
        .unwrap();
    }

    handle.flush().unwrap();
}

fn main() {
    let _args = Args::parse();
    let config = load_or_create_config(Path::new(CONFIG_PATH));
    let art = load_art(&*config.art_path);
    print(&art, &config.accent_color, &config.base_color);
}
