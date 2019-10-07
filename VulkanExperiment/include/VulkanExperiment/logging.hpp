#pragma once

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

inline void info(const std::string fmt, ...);

/**
 * @brief What error messages to log
 *
 */
enum ELogLevel { OFF = 0, ERROR = 1, WARN = 2, INFO = 3, DEBUG = 4, TRACE = 5 };

/**
 * @brief This interface allows you to log some information.
 *
 */
class ILogger {
public:
  inline virtual void trace(std::string message) = 0;
  inline virtual void debug(std::string message) = 0;
  inline virtual void info(std::string message) = 0;
  inline virtual void error(std::string message) = 0;
};

/**
 * @brief This bad boii creates loggers on demand.
 */
class ILoggerFactory {
  /**
   * @brief Get the Instance object
   *
   * @param loggerName
   * @return std::unique_ptr<ILogger>
   */
  virtual std::unique_ptr<ILogger>
  getInstance(const std::string loggerName) = 0;
};

class NoOpLogger : public ILogger {
  inline virtual void trace(std::string message) override {}
  inline virtual void debug(std::string message) override {}
  inline virtual void info(std::string message) override {}
  inline virtual void error(std::string message) override {}
};

/**
 * @brief this class, like the name implies does not log anything.  this is a
 * useful setting to disable logging everywhere.
 */
class NoOpLoggerFactory : public ILoggerFactory {
  virtual std::unique_ptr<ILogger> getInstance(const std::string loggerName) {
    return std::unique_ptr<NoOpLogger>{};
  }
};

std::unique_ptr<ILoggerFactory>
createStdStreamLoggerFactory(const ELogLevel logLevel = ELogLevel::INFO);

} // namespace vke::log