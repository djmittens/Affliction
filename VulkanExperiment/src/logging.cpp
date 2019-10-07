#include "VulkanExperiment/logging.hpp"
#include "boost/format/format_implementation.hpp"
#include "boost/log/trivial.hpp"
#include <cstdio>
#include <iostream>


namespace vke::log {
inline void info(const std::string fmt, ...) {
  std::cout << "Hello World";
#if defined(ENABLE_LOG)
  char buf[256];
  va_list args;
  va_start(args, fmt);

  std::vsnprintf(fmt.data(), len + 1, fmt, va_list);
  va_end(args);

  std::string out(buf);
  BOOST_LOG_TRIVIAL(info) << out;
#endif
}
} // namespace vke::log