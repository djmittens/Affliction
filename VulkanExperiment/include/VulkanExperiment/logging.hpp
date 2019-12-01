#pragma once

#include "vke.hpp"
#include <memory>
#include <string>

/**
 * @brief In this namespace, i do the impossible, I take on the problem of
 * dynamic logging. Wish me luck on this super challenge.
 *
 */
namespace vke::log {
static const char TAB = '\t';
static const char ENDL = '\n';

// VKE_EXPORT inline void info(const std::string fmt, ...);
// VKE_EXPORT inline void debug(const std::string fmt, ...);

template <typename... Ts> std::string format(const char *p_fmt, Ts... p_args) {
  // Have to double call this function in order for the length to workout.
  const int len = std::vsnprintf(NULL, 0, p_fmt.c_str(), p_args...);
  std::string out;
  out.resize(len + 1);
  std::vsnprintf(out.data(), out.size(), p_fmt.c_str(), p_args...);
  return out;
}

// template <typename... Args>
// inline void info(const char *p_fmt, Args... p_args) {}

std::ostream &info();
std::ostream &debug();

// template <typename... Args>
// inline void error(const char *p_fmt, Args... p_args) {
// #if defined(ENABLE_LOG)
//   BOOST_LOG_TRIVIAL(error) << format(p_fmt, p_args);
// #endif
// }

// template <typename... Args>
// inline void debug(const char *p_fmt, Args... p_args) {
// #if defined(ENABLE_LOG)
//   BOOST_LOG_TRIVIAL(debug) << format(p_fmt, p_args);
// #endif
// }

} // namespace vke::log