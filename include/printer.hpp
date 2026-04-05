#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <optional>

#include "art.hpp"
#include "config.hpp"

namespace printer {

void Print(const std::optional<art::Art>& art, const config::Color base_color,
           const config::Color accent_color);

}  // namespace printer

#endif
