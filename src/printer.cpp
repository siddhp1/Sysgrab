#include "printer.hpp"

#include <iostream>
#include <optional>
#include <string>

#include "art.hpp"
#include "config.hpp"

namespace printer {

// TODO: Optimize this by building a string and printing all at once
void Print(const std::optional<art::Art>& art, const config::Color base_color,
           const config::Color accent_color) {
  std::cout << "\033[38;2;" << static_cast<int>(accent_color.at(0)) << ";"
            << static_cast<int>(accent_color.at(1)) << ";"
            << static_cast<int>(accent_color.at(2)) << "m";

  if (art) {
    for (const std::string& string : *art) {
      std::cout << string << '\n';
    }
  }

  std::cout << "\033[0m";
}

}  // namespace printer
