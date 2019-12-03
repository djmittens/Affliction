#pragma once

#include "vke.hpp"
#include <memory>
#include <stdarg.h>
#include <string>

// keeping this guy around as an example of variadic macros.
// #define VKE_INFO(fmt, ...) \
//   vke::log::newEvent(vke::log::Level::INFO, fmt, __VA_ARGS__)

/**
 * @brief In this namespace, i do the impossible, I take on the problem of
 * dynamic logging. Wish me luck on this super challenge.
 *
 */
namespace vke::log {
static const char TAB = '\t';
static const char ENDL = '\n';

enum Level { OFF = 0, ERR = 1, WARN = 2, INFO = 3, DEBUG = 4, TRACE = 5 };

std::string toString(Level lvl);

struct Event {
  const std::string message;
  const Level level;
  const long timestamp;
};

class ILogger {
public:
  virtual ~ILogger() = 0;

  virtual Level logLevel() = 0;
  virtual void log(Event evt) = 0;
};

std::string format(const char *p_fmt, ...);

// template <typename... Ts>
// inline Event newEvent(Level level, const std::string fmt, Ts... args) {
//   return Event{format(fmt.c_str(), args...), level, -1};
// }

class LazyLogger {
private:
  std::shared_ptr<ILogger> m_logger;
  Level m_level;

public:
  LazyLogger(std::shared_ptr<ILogger> logger) : m_logger(logger) {
    m_level = logger->logLevel();
  }

  inline void error(const std::string message) { lazyLog(Level::ERR, message); }

  template <typename... Ts>
  inline void error(const std::string fmt, Ts... args) {
    lazyLog(Level::ERR, fmt, args...);
  }

  inline void warn(const std::string msg) { lazyLog(Level::WARN, msg); }

  template <typename... Ts>
  inline void warn(const std::string fmt, Ts... args) {
    lazyLog(Level::WARN, fmt, args...);
  }

  inline void info(const std::string fmt) { lazyLog(Level::INFO, fmt); }

  template <typename... Ts>
  inline void info(const std::string fmt, Ts... args) {
    lazyLog(Level::INFO, fmt, args...);
  }

  inline void debug(const std::string fmt) { lazyLog(Level::DEBUG, fmt); }

  template <typename... Ts>
  inline void debug(const std::string fmt, Ts... args) {
    lazyLog(Level::DEBUG, fmt, args...);
  }

  inline void trace(const std::string fmt) { lazyLog(Level::TRACE, fmt); }

  template <typename... Ts>
  inline void trace(const std::string fmt, Ts... args) {
    lazyLog(Level::TRACE, fmt, args...);
  }

  template <typename... Ts>
  inline void lazyLog(Level level, const std::string fmt, Ts... args) {
    lazyLog(level, format(fmt.c_str(), args...));
  }

  inline void lazyLog(Level level, const std::string message) {
    if (m_level >= level) {
      m_logger->log(Event{message, level, -1});
    }
  }
};

std::unique_ptr<ILogger> crapLogger();

} // namespace vke::log