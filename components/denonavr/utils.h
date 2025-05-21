#pragma once

#include <charconv>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

namespace esphome {
namespace denonavr {

inline bool in_array(const std::string &value,
                     const std::vector<std::string> &array) {
  return std::find(array.begin(), array.end(), value) != array.end();
}

inline bool is_integer(const std::string &s) {
  int value;
  auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
  return ec == std::errc() && ptr == s.data() + s.size();
}

inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

inline void trim(std::string &s) {
  rtrim(s);
  ltrim(s);
}

inline std::string format_volume(float value) {
    int intPart = static_cast<int>(value);
    float fracPart = value - intPart;
    char buffer[8];

    if (fracPart == 0.5f) {
        int finalValue = static_cast<int>(value * 10);
        snprintf(buffer, sizeof(buffer), "%03d", finalValue);
    } else {
        int rounded = (fracPart < 0.5f) ? static_cast<int>(std::floor(value)) : static_cast<int>(std::ceil(value));
        snprintf(buffer, sizeof(buffer), "%02d", rounded);
    }

    return std::string(buffer);
}

} // namespace denonavr
} // namespace esphome
