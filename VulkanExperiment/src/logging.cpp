#include "VulkanExperiment/logging.hpp"
#include "boost/format/format_implementation.hpp"
#include "boost/log/trivial.hpp"
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <string>
// #include <move>


namespace vke::log {
inline std::string format(const std::string pFmt, va_list pArgs) {

  va_list acopy;
  va_copy(acopy, pArgs);
  const int len = std::vsnprintf(NULL, 0, pFmt.c_str(), acopy);
  va_end(acopy);

  std::string out;
  out.resize(len + 1);

  std::vsnprintf(out.data(), out.size(), pFmt.c_str(), pArgs);

  return out;
}

inline void info(const std::string pFmt, ...) {
#if defined(ENABLE_LOG)
  va_list args;
  va_start(args, pFmt);
  BOOST_LOG_TRIVIAL(info) << format(pFmt.c_str(), args); 
  va_end(args);
#endif
}

inline void debug(const std::string pFmt, ...) {
#if defined(ENABLE_LOG)
  va_list args;
  va_start(args, pFmt);
  BOOST_LOG_TRIVIAL(debug) << format(pFmt.c_str(), args); 
  va_end(args);
#endif
}


} // namespace vke::log