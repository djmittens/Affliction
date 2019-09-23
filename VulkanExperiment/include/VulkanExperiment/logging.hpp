#pragma once

namespace vke::platform {
static const char TAB = '\t';
static const char ENDL = '\n';

class ILogger {
public:
  inline virtual void info() = 0;
};
} // namespace vke::platform