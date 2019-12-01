#pragma once

#include "vke.hpp"
#include <memory>
#include <string>

#define VKE_LOG(fmt, ...)                                                      \
  vke::log::newEvent(__FILE__, __LINE__, fmt, __VA_ARGS__)

/**
 * @brief In this namespace, i do the impossible, I take on the problem of
 * dynamic logging. Wish me luck on this super challenge.
 *
 */
namespace vke::log {
static const char TAB = '\t';
static const char ENDL = '\n';

enum LogLevel { OFF = 0, ERROR = 1, WARN = 2, INFO = 3, DEBUG = 4, TRACE = 5 };

struct LogEvent {
  const std::string message;
  const std::string file;
  const int lineNumber;
  const long timestamp;
};

class ILogger {
public:
  virtual ~ILogger() = 0;

  virtual LogLevel logLevel() = 0;
  virtual void trace(LogEvent evt) = 0;
  virtual void info(LogEvent evt) = 0;
  virtual void debug(LogEvent evt) = 0;
  virtual void warn(LogEvent evt) = 0;
  virtual void error(LogEvent evt) = 0;
};

// VKE_EXPORT inline void info(const std::string fmt, ...);
// VKE_EXPORT inline void debug(const std::string fmt, ...);

class LazyLogger {
private:
  std::shared_ptr<ILogger> m_logger;
  LogLevel m_level;

public:
  LazyLogger(std::shared_ptr<ILogger> logger) : m_logger(logger) {
    m_level = logger->logLevel();
  }

  inline void error(LogEvent evt) {
    if (m_level >= LogLevel::ERROR) {
      m_logger->info(evt);
    }
  };

  inline void warn(LogEvent evt) {
    if (m_level >= LogLevel::WARN) {
      m_logger->warn(evt);
    }
  };

  inline void info(LogEvent evt) {
    if (m_level >= LogLevel::INFO) {
      m_logger->info(evt);
    }
  };

  inline void debug(LogEvent evt) {
    if (m_level >= LogLevel::DEBUG) {
      m_logger->debug(evt);
    }
  };

  inline void trace(LogEvent evt) {
    if (m_level >= LogLevel::TRACE) {
      m_logger->trace(evt);
    }
  };
};

template <typename... Ts>
std::string format(const std::string p_fmt, Ts... p_args) {
  // Have to double call this function in order for the length to workout.
  const auto ptr = p_fmt.c_str();
  const int len = std::vsnprintf(NULL, 0, ptr, std::forward<Ts>(p_args)...);
  std::string out;
  out.resize(len + 1);
  std::vsnprintf(out.data(), out.size(), ptr, std::forward<Ts>(p_args)...);
  return out;
}

inline LogEvent newEvent(const std::string file, const int lineNumber,
                         const std::string message) {
  return LogEvent{message, file, lineNumber, -1};
}

template <typename... Ts>
inline LogEvent newEvent(const std::string file, const int lineNumber,
                         const std::string fmt, Ts... args) {
  return LogEvent{format(fmt, args...), file, lineNumber, -1};
}

std::unique_ptr<ILogger> crapLogger();

// template <typename... Args>
// inline void info(const char *p_fmt, Args... p_args) {}

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