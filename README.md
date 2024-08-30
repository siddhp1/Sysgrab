# Sysgrab

Sysgrab is a lightweight and customizable system information tool for fetching and displaying system details such as OS, architecture, CPU, memory, and more in the terminal.

<p align="center"><a><img width="800" alt="Thumbnail Image of Sysgrab" src="https://www.siddhp.me/_next/image?url=%2Fsysgrab.png&w=3840&q=75"></a></p>

Users can configure Sysgrab's base and accent colors, and add ASCII art of their choice through the CLI or by directly editing the configuration files.

Sysgrab is compatible with all Linux distributions.

## Installation

To install Sysgrab, follow these steps:

1. **Download the latest release**:

    Download the latest version of `sysgrab-release.zip` from the [releases page](https://github.com/siddhp1/Sysgrab/releases/) and extract it.

2. **Add Sysgrab to your PATH**:

    Add the extracted directory to your system PATH to make `sysgrab` accessible from anywhere in the terminal.

    Add this line to `.bashrc` or `.zshrc` to make the change persistent.

    ```bash
    export PATH=$PATH:/path/to/sysgrab-directory
    ```

    

## Usage

Run Sysgrab with the following command:

```bash
sysgrab [OPTIONS]
```

**Options:**
```text
  (no option)                   Display system information
  -h, --help                    Show a help message and exit
  -v, --version                 Display version information and exit
  -b, --base-color [r,g,b]      Set base color in the format r,g,b
  -a, --accent-color [r,g,b]    Set accent color in the format r,g,b
```

## Configuration

To configure Sysgrab, follow these steps:

1. **Configure colors**:

    To configure the colors, edit the `config.txt` file, or run the following command with rgb values of choice:

    ```bash
    sysgrab --base-color r,g,b --accent-color r,g,b
    ```

2. **Add art**:

    To configure the art, paste any ASCII art in the `art.txt` file.

## License

This project is licensed under the MIT License.