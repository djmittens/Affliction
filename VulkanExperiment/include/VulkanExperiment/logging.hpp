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

VKE_EXPORT inline void info(const std::string fmt, ...);
VKE_EXPORT inline void debug(const std::string fmt, ...);

} // namespace vke::log