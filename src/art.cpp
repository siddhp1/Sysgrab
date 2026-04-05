#include "art.hpp"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "logger.hpp"

namespace art {

// TODO: Consider stream-and-printing to avoid allocations
// TODO: Consider running this in parallel with other file access operations

auto Read(const std::filesystem::path& path) -> std::optional<Art> {
  std::ifstream file(path);
  if (!file) {
    logger::Warning("Failed to read art file \"{}\"", path.string());
    return std::nullopt;
  }

  Art art;
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    art.push_back(line);
  }

  if (!file.eof()) {
    logger::Warning("Failed while reading art file \"{}\"", path.string());
    return std::nullopt;
  }

  if (art.empty()) {
    return std::nullopt;
  }

  return art;
}

}  // namespace art
