#include "VulkanExperiment/logging.hpp"
#include <cstdarg>
#include <cstdio>
#include <iosfwd>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <string>
#include <vector>
#include <fmt/printf.h>
#include <fmt/format.h>
#include <fmt/color.h>

namespace vke::log {

ILogger::~ILogger() {}

std::string toString(Level lvl) {
  std::string result = "OFF";
  switch(lvl) {
    case Level::ERR:
      result = "ERROR";
      break;
    case Level::WARN:
      result = "WARN";
      break;
    case Level::INFO:
      result = "INFO";
      break;
    case Level::DEBUG:
      result = "DEBUG";
      break;
    case Level::TRACE:
      result = "TRACE";
      break;
  }
  return result;
}

std::string format(const char* p_fmt, ...) {
  va_list args;
  va_start(args, p_fmt);
  // Have to double call this function in order for the length to workout.
  const int len = std::vsnprintf(NULL, 0, p_fmt, args);
  std::string out;
  out.resize(len + 1);
  std::vsnprintf(out.data(), out.size(), p_fmt , args);
  va_end(args);
  return out;
}

class CoutLogger : public ILogger {

public:
  ~CoutLogger() = default;
  virtual Level logLevel() override { return Level::TRACE; }
  virtual void log(Event evt) {
    // std::cout << "[" << std::setw(5) << toString(evt.level) <<  "]" << TAB << evt.message << ENDL;
    // auto lvl = fmt::format(fmt::emphasis::bold | fg(fmt::color::red), toString(evt.level));
    auto lvl = toString(evt.level);

    fmt::print("[{:<5}] {}\n", lvl, evt.message);
  }
};

std::unique_ptr<ILogger> crapLogger() {
  return std::unique_ptr<ILogger>(new CoutLogger());
}

} // namespace vke::log