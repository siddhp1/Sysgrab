# Sysgrab

Sysgrab is a lightweight and customizable system information tool for fetching and displaying system details such as OS, architecture, CPU, memory, and more in the terminal.

<p align="center"><a><img width="800" alt="Thumbnail Image of Sysgrab" src="https://www.siddhp.me/_next/image?url=%2Fsysgrab.png&w=3840&q=75"></a></p>

Users can configure Sysgrab's colors, the ordering of datpoints and add ASCII art of their choice by editing the configuration files.

Sysgrab is compatible with all Linux distributions.

## Installation

To install Sysgrab, follow these steps:

1. **Download the latest release**:

    Download the latest version of `sysgrab-release.zip` from the [releases page](https://github.com/siddhp1/Sysgrab/releases/) and extract it.

2. **Add Sysgrab to your PATH**:

    Add the extracted directory to your system PATH to make `sysgrab` accessible from anywhere in the terminal.

    Add this line to your shell configuration to make the change persistent:

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
  -d, --delete-logs             Delete logs
```

## Configuration

To configure Sysgrab, follow these steps:

1. **Configure colors and data ordering**:

    To configure the colors and ordering, edit the `config.yaml` file.

2. **Add art**:

    To configure the art, create a text file with ASCII art, and put the file name in the `config.yaml` file.

## License

This project is licensed under the MIT License.