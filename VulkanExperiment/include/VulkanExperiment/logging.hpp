#pragma once

#include <memory>
#include <string>

namespace vke::platform::logging {
static const char TAB = '\t';
static const char ENDL = '\n';

/**
 * @brief This interface allows you to log some information.
 *
 */
class ILogger {
public:
  inline virtual void trace() = 0;
  inline virtual void debug() = 0;
  inline virtual void info() = 0;
};

class ILoggerFactory {

  virtual std::unique_ptr<ILogger>
  getInstance(const std::string &loggerName) = 0;
};

} // namespace vke::platform::logging