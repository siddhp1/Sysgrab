#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <array>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <vector>

#include "data.hpp"
#include "logger.hpp"

namespace config {

struct Config {
  std::filesystem::path art_path{};
  std::array<uint8_t, 3> base_color{255, 255, 255};
  std::array<uint8_t, 3> accent_color{255, 255, 255};
  std::vector<data::Datum> ordering{
      data::Datum::kOs,       data::Datum::kArchitecture, data::Datum::kKernel,
      data::Datum::kComputer, data::Datum::kShell,        data::Datum::kUptime,
      data::Datum::kCpu,      data::Datum::kMemory,
  };
  logger::Level log_level{logger::Level::kError};
};

auto Read(const std::filesystem::path& path) -> Config;

}  // namespace config

#endif
