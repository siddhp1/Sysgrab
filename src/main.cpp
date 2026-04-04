#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "config.hpp"
#include "logger.hpp"

// TODO: Update this, or even make it more general
enum class ArtError : uint8_t { FILE_OPEN_FAIL };

// TODO: Consider opening file up concurrently while printing to avoid
// loading into memory
// TODO: Consider running this in parallel with other file access operations
auto get_art(const std::filesystem::path& path)
    -> std::expected<std::vector<std::string>, ArtError> {
  std::ifstream file(path);
  if (!file) {
    return std::unexpected(ArtError::FILE_OPEN_FAIL);
  }

  std::vector<std::string> art{};
  std::string line;
  while (std::getline(file, line)) {
    art.push_back(line);
  }

  return art;
}

void print(const std::vector<std::string>& art) {
  for (const std::string& string : art) {
    std::cout << string << '\n';
  }
}

int main(int argc, char* argv[]) {
  auto start = std::chrono::high_resolution_clock::now();

  const std::vector<std::string_view> args(argv + 1, argv + argc);

  for (const auto& arg : args) {
    if (arg == "--help" || arg == "-h") {
      std::cout << "Usage: sysgrab\n";
      std::exit(EXIT_SUCCESS);
    } else if (arg == "--version" || arg == "-v") {
      std::cout << "Version" << SYSGRAB_VER << '\n';
      std::exit(EXIT_SUCCESS);
    } else {
      std::cerr << "Unknown argument: " << arg << '\n';
      std::exit(EXIT_FAILURE);
    }
  }
  const auto config = config::Read(std::filesystem::path("config.cfg"));
  logger::min_level = config.log_level;

  // TODO: Fix these debug logs
  // logger::Debug("{}", config.art_path.string());
  // logger::Debug("{}", config.base_color);
  // logger::Debug("{}", config.accent_color);
  // logger::Debug("{}", config.ordering);
  // logger::Debug("{}", config.log_level);

  const auto art_result = get_art(config.art_path);
  if (!art_result) {
    // TODO: Handle the error here
  }

  const auto& art = art_result.value();
  print(art);

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  logger::Debug("Sysgrab finished in {}", duration);
}
