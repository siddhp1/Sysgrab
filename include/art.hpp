#ifndef ART_HPP
#define ART_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace art {

using Art = std::vector<std::string>;

auto Read(const std::filesystem::path& path) -> std::optional<Art>;

}  // namespace art

#endif
