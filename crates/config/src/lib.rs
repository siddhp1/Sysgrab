use std::fs;
use std::path::{Path, PathBuf};

use serde::{Deserialize, Serialize};

use data::DataPointKind;

pub const CONFIG_PATH: &str = "config.toml";

#[derive(Serialize, Deserialize, Debug, Eq, PartialEq)]
#[serde(default)]
pub struct Config {
    pub art_path: PathBuf,
    pub accent_color: [u8; 3],
    pub base_color: [u8; 3],
    pub ordering: Vec<DataPointKind>,
}

impl Default for Config {
    fn default() -> Self {
        Self {
            art_path: PathBuf::from("art.txt"),
            accent_color: [20, 200, 255],
            base_color: [255, 255, 255],
            ordering: vec![
                DataPointKind::Os,
                DataPointKind::Architecture,
                DataPointKind::Kernel,
                DataPointKind::Computer,
                DataPointKind::Shell,
                DataPointKind::Uptime,
                DataPointKind::Cpu,
                DataPointKind::Memory,
            ],
        }
    }
}

pub fn load_or_create_config(path: &Path) -> Config {
    // If file does not exist, use default
    if !path.exists() {
        let default_config = Config::default();
        let toml_string =
            toml::to_string_pretty(&default_config).expect("Failed to serialize config");
        fs::write(path, toml_string).expect("Failed to write config");
        return default_config;
    }

    // Otherwise read string and parse
    let content = fs::read_to_string(path).expect("Failed to read config");
    toml::from_str(&content).expect("Failed to parse config")
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::{tempdir, TempDir};

    fn create_temp_config(contents: &str) -> (TempDir, PathBuf) {
        let temp_dir = tempdir().expect("Failed to create temporary directory");
        let config_path = temp_dir.path().join(CONFIG_PATH);

        fs::write(&config_path, contents).expect("Failed to write test config");
        (temp_dir, config_path)
    }

    #[test]
    fn load_config_returns_values_from_complete_file() {
        let contents = r#"
accent_color = [1, 2, 3]
art_path = "custom-art.txt"
base_color = [4, 5, 6]
ordering = ["Memory", "Cpu", "Os"]
"#;
        let (_temp_dir, config_file) = create_temp_config(contents);
        let config = load_or_create_config(&config_file);

        let expected = Config {
            accent_color: [1, 2, 3],
            art_path: PathBuf::from("custom-art.txt"),
            base_color: [4, 5, 6],
            ordering: vec![DataPointKind::Memory, DataPointKind::Cpu, DataPointKind::Os],
        };

        assert_eq!(config, expected);
    }

    #[test]
    fn load_config_uses_defaults_for_missing_fields() {
        let contents = r#"
accent_color = [100, 150, 200]
art_path = "partial-art.txt"
"#;
        let (_temp_dir, config_file) = create_temp_config(contents);
        let config = load_or_create_config(&config_file);
        let defaults = Config::default();

        assert_eq!(config.accent_color, [100, 150, 200]);
        assert_eq!(config.art_path, PathBuf::from("partial-art.txt"));
        assert_eq!(config.base_color, defaults.base_color);
        assert_eq!(config.ordering, defaults.ordering);
    }

    #[test]
    fn load_config_returns_defaults_for_empty_file() {
        let (_temp_dir, config_file) = create_temp_config("");
        let config = load_or_create_config(&config_file);

        assert_eq!(config, Config::default());
    }

    #[test]
    fn load_config_creates_default_file_when_file_is_missing() {
        let temp_dir = tempdir().expect("Failed to create temporary directory");
        let config_path = temp_dir.path().join(CONFIG_PATH);

        assert!(!config_path.exists());

        let config = load_or_create_config(&config_path);

        assert_eq!(config, Config::default());
        assert!(config_path.exists());

        let generated_contents =
            fs::read_to_string(&config_path).expect("Failed to read generated config");

        let generated_config: Config =
            toml::from_str(&generated_contents).expect("Failed to parse config");

        assert_eq!(generated_config, Config::default());
    }
}
