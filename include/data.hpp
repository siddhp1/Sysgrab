#ifndef DATA_HPP
#define DATA_HPP

#include <cstdint>

namespace data {

enum class Datum : uint8_t {
  kArchitecture,
  kComputer,
  kCpu,
  kKernel,
  kMemory,
  kOs,
  kShell,
  kUptime,
  kCount  // Count sentinel
};

}  // namespace data

#endif
