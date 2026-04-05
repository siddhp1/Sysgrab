#include "config.hpp"

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "data.hpp"
#include "logger.hpp"

namespace config {

constexpr std::size_t kDigitNum = 3;
constexpr std::size_t kDigitLen = 2;
constexpr int kHexBase = 16;
constexpr uint8_t kCharMax = 0xFFU;

constexpr std::string_view kArtPathKey = "art_path";
constexpr std::string_view kBaseColorKey = "base_color";
constexpr std::string_view kAccentColorKey = "accent_color";
constexpr std::string_view kOrderingKey = "ordering";
constexpr std::string_view kLogLevelKey = "log_level";

enum class Error : uint8_t {
  kInvalidColor,
  kInvalidLine,
  kInvalidLogLevel,
  kInvalidOrdering,
};

constexpr auto IsSpace(char c) -> bool {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

auto Trim(std::string_view sv) -> std::string_view {
  while (!sv.empty() && IsSpace(sv.front())) {
    sv.remove_prefix(1);
  }
  while (!sv.empty() && IsSpace(sv.back())) {
    sv.remove_suffix(1);
  }
  return sv;
}

auto SplitKeyVal(std::string_view line)
    -> std::expected<std::pair<std::string_view, std::string_view>, Error> {
  const std::size_t pos = line.find('=');
  if (pos == std::string_view::npos) {
    return std::unexpected(Error::kInvalidLine);
  }
  const std::string_view key = Trim(line.substr(0, pos));
  const std::string_view val = Trim(line.substr(pos + 1));
  if (key.empty()) {
    return std::unexpected(Error::kInvalidLine);
  }
  return std::pair{key, val};
}

auto ParseDatum(std::string_view token) -> std::expected<data::Datum, Error> {
  token = Trim(token);
  if (token == "os") {
    return data::Datum::kOs;
  }
  if (token == "architecture") {
    return data::Datum::kArchitecture;
  }
  if (token == "kernel") {
    return data::Datum::kKernel;
  }
  if (token == "computer") {
    return data::Datum::kComputer;
  }
  if (token == "shell") {
    return data::Datum::kShell;
  }
  if (token == "uptime") {
    return data::Datum::kUptime;
  }
  if (token == "cpu") {
    return data::Datum::kCpu;
  }
  if (token == "memory") {
    return data::Datum::kMemory;
  }
  return std::unexpected(Error::kInvalidOrdering);
}

auto ParseOrdering(std::string_view str)
    -> std::expected<std::vector<data::Datum>, Error> {
  std::vector<data::Datum> ordering;
  ordering.reserve(static_cast<std::size_t>(data::Datum::kCount));

  std::size_t start = 0;
  while (start <= str.size()) {
    const std::size_t comma = str.find(',', start);
    const std::size_t end =
        (comma == std::string_view::npos) ? str.size() : comma;

    std::string_view token = str.substr(start, end - start);
    token = Trim(token);
    if (!token.empty()) {
      auto datum = ParseDatum(token);
      if (!datum) {
        return std::unexpected(datum.error());
      }
      ordering.push_back(*datum);
    }

    if (comma == std::string_view::npos) {
      break;
    }
    start = comma + 1;
  }

  return ordering;
}

auto ParseLogLevel(std::string_view str)
    -> std::expected<logger::Level, Error> {
  str = Trim(str);
  if (str == "debug") {
    return logger::Level::kDebug;
  }
  if (str == "info") {
    return logger::Level::kInfo;
  }
  if (str == "warning") {
    return logger::Level::kWarning;
  }
  if (str == "error") {
    return logger::Level::kError;
  }
  if (str == "off") {
    return logger::Level::kOff;
  }
  return std::unexpected(Error::kInvalidLogLevel);
}

auto ParseColor(std::string_view str) -> std::expected<Color, Error> {
  str = Trim(str);
  if (str.size() != kDigitLen * kDigitNum) {
    return std::unexpected(Error::kInvalidColor);
  }

  Color color{};
  for (std::size_t i = 0; i < kDigitNum; ++i) {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const char* begin = str.data() + (i * kDigitLen);
    const char* end = begin + kDigitLen;
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    unsigned int byte = 0;
    auto [ptr, ec] = std::from_chars(begin, end, byte, kHexBase);
    if (ec != std::errc() || ptr != end || byte > kCharMax) {
      return std::unexpected(Error::kInvalidColor);
    }
    color.at(i) = static_cast<uint8_t>(byte);
  }

  return color;
}

auto Read(const std::filesystem::path& path) -> Config {
  Config config{};

  std::ifstream file(path);
  if (!file) {
    logger::Warning("Failed to read config file \"{}\"", path.string());
    return config;
  }

  std::string line;
  uint32_t line_num = 0;
  while (std::getline(file, line)) {
    ++line_num;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    const std::string_view line_view = Trim(std::string_view(line));
    if (line_view.empty() || line_view.front() == '#') {
      continue;
    }

    auto kv = SplitKeyVal(line_view);
    if (!kv) {
      logger::Warning("Failed to parse config file line \"{}\"", line_view);
      continue;
    }

    const auto [key, val] = *kv;
    if (key == kArtPathKey) {
      config.art_path = std::filesystem::path(val);
    } else if (key == kBaseColorKey) {
      auto color = ParseColor(val);
      if (!color) {
        logger::Warning("Failed to parse color \"{}\"", val);
        continue;
      }
      config.base_color = *color;
    } else if (key == kAccentColorKey) {
      auto color = ParseColor(val);
      if (!color) {
        logger::Warning("Failed to parse color \"{}\"", val);
        continue;
      }
      config.accent_color = *color;
    } else if (key == kOrderingKey) {
      auto ordering = ParseOrdering(val);
      if (!ordering) {
        logger::Warning("Failed to parse ordering \"{}\"", val);
        continue;
      }
      config.ordering = std::move(*ordering);
    } else if (key == kLogLevelKey) {
      auto lvl = ParseLogLevel(val);
      if (!lvl) {
        logger::Warning("Failed to parse log level \"{}\"", val);
        continue;
      }
      config.log_level = *lvl;
    } else {
      logger::Warning("Unknown key \"{}\"", key);
    }
  }

  return config;
}

}  // namespace config
