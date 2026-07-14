use std::fs;
use std::path::Path;

#[derive(Debug, Eq, PartialEq)]
pub struct Art {
    pub data: Vec<String>,
}

pub fn load_art(path: &Path) -> Art {
    let contents = fs::read_to_string(path).expect("Failed to read art");
    Art {
        data: contents.lines().map(String::from).collect(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::tempdir;

    const ART_PATH: &str = "art.txt";

    #[test]
    fn load_art_returns_data_from_file() {
        let temp_dir = tempdir().expect("Failed to create temporary directory");
        let art_path = temp_dir.path().join(ART_PATH);

        let contents = " /\\_/\\\n( o.o )\n > ^ <";
        fs::write(&art_path, contents).expect("Failed to write test art");

        let art = load_art(&art_path);

        let expected = Art {
            data: vec![
                String::from(" /\\_/\\"),
                String::from("( o.o )"),
                String::from(" > ^ <"),
            ],
        };

        assert_eq!(art, expected);
    }

    #[test]
    #[should_panic(expected = "Failed to read art")]
    fn load_art_panics_when_file_is_missing() {
        let temp_dir = tempdir().expect("Failed to create temporary directory");
        let missing_path = temp_dir.path().join(ART_PATH);

        assert!(!missing_path.exists());

        load_art(&missing_path);
    }
}
