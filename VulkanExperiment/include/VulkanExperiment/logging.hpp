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
  virtual void log(const Event &evt) = 0;
};

std::string format(const char *p_fmt, ...);

// template <typename... Ts>
// inline Event newEvent(Level level, const std::string fmt, Ts... args) {
//   return Event{format(fmt.c_str(), args...), level, -1};
// }

class LazyLogger {
private:
  std::unique_ptr<ILogger> m_logger;

public:
  LazyLogger(std::unique_ptr<ILogger> logger) : m_logger(std::move(logger)) {
  }

  inline void error (const std::string &message) {
    lazyLog(Level::ERR, message);
  }
  inline void warn (const std::string &message) {
    lazyLog(Level::WARN, message);
  }
  inline void info (const std::string &message) {
    lazyLog(Level::INFO, message);
  }
  inline void debug (const std::string &message) {
    lazyLog(Level::DEBUG, message);
  }
  inline void trace (const std::string &message) {
    lazyLog(Level::TRACE, message);
  }

  inline void lazyLog(const Level level, const std::string &message) { 
    if (m_logger->logLevel() >= level) {
      m_logger->log(Event{message, level, -1});
    }
  }

  inline void lazyLog(const Level level, const Event &event) {

    if (m_logger->logLevel() >= level) {
      m_logger->log(event);
    }
  }
};

std::unique_ptr<ILogger> crapLogger();
std::unique_ptr<ILogger> fileLogger(const std::string &filename);

} // namespace vke::log