#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

#include "art.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "printer.hpp"

int main(int argc, char* argv[]) {  // NOLINT(bugprone-exception-escape)
  auto start = std::chrono::high_resolution_clock::now();

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const std::vector<std::string_view> args(argv + 1, argv + argc);

  // There will only be one thread running at this point
  // NOLINTBEGIN(concurrency-mt-unsafe)
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
  // NOLINTEND(concurrency-mt-unsafe)

  const auto config = config::Read(std::filesystem::path("config.cfg"));
  logger::min_level = config.log_level;

  // TODO: Fix these debug logs
  // logger::Debug("{}", config.art_path.string());
  // logger::Debug("{}", config.base_color);
  // logger::Debug("{}", config.accent_color);
  // logger::Debug("{}", config.ordering);
  // logger::Debug("{}", config.log_level);
  const auto art = art::Read(config.art_path);

  if (art) {
    printer::Print(art, config.base_color, config.accent_color);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  logger::Debug("Sysgrab finished in {}", duration);
}
