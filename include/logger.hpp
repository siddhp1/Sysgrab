#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdint>
#include <format>
#include <iostream>
#include <mutex>
#include <source_location>
#include <string>

namespace logger {

enum class Level : uint8_t { kDebug, kInfo, kWarning, kError, kOff };

constexpr std::string_view GetLevelStr(Level l) {
  switch (l) {
    case Level::kDebug:
      return "DEBUG";
    case Level::kInfo:
      return "INFO";
    case Level::kWarning:
      return "WARNING";
    case Level::kError:
      return "ERROR";
    case Level::kOff:
      return "OFF";
    default:
      return "UNKNOWN";
  }
}

inline std::atomic<Level> min_level = Level::kDebug;

inline std::mutex& GetMutex() {
  static std::mutex m;
  return m;
}

template <Level L, class... Args>
inline void Log(
    std::format_string<Args...> fmt, Args&&... args,
    const std::source_location loc = std::source_location::current()) {
  if (L < min_level.load(std::memory_order_relaxed)) {
    return;
  }

  std::scoped_lock lock(GetMutex());
  std::cerr << GetLevelStr(L) << ": "
            << std::format(fmt, std::forward<Args>(args)...) << " ("
            << loc.file_name() << ":" << loc.line() << ")\n";
}

template <class... Args>
inline void Debug(
    std::format_string<Args...> f, Args&&... a,
    const std::source_location loc = std::source_location::current()) {
  Log<Level::kDebug>(f, std::forward<Args>(a)..., loc);
}
template <class... Args>
inline void Info(
    std::format_string<Args...> f, Args&&... a,
    const std::source_location loc = std::source_location::current()) {
  Log<Level::kInfo>(f, std::forward<Args>(a)..., loc);
}
template <class... Args>
inline void Warning(
    std::format_string<Args...> f, Args&&... a,
    const std::source_location loc = std::source_location::current()) {
  Log<Level::kWarning>(f, std::forward<Args>(a)..., loc);
}
template <class... Args>
inline void Error(
    std::format_string<Args...> f, Args&&... a,
    const std::source_location loc = std::source_location::current()) {
  Log<Level::kError>(f, std::forward<Args>(a)..., loc);
}

}  // namespace logger

#endif
