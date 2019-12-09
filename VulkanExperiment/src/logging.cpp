#include "VulkanExperiment/logging.hpp"
#include <cstdarg>
#include <cstdio>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace vke::log {

ILogger::~ILogger() {}

std::string toString(Level lvl) {
  std::string result = "OFF";
  switch (lvl) {
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


class CoutLogger : public ILogger {

public:
  ~CoutLogger() = default;
  virtual Level logLevel() override { return Level::TRACE; }
  virtual void log(const Event &evt) {
    auto lvl = toString(evt.level);

    fmt::print("[{:<5}] {}\n", lvl, evt.message);
  }
};

class SpdLogger : public ILogger {
  std::shared_ptr<spdlog::logger> m_logger;

public:
  SpdLogger(std::shared_ptr<spdlog::logger> p_logger) : m_logger(p_logger) {}

  ~SpdLogger() = default;
  virtual Level logLevel() override { return Level::TRACE; }
  virtual void log(const Event &evt) { 
    switch(evt.level) {
      case Level::ERR:
        m_logger->error(evt.message);
        break;
      case Level::WARN:
        m_logger->warn(evt.message);
        break;
      case Level::INFO:
        m_logger->info(evt.message);
        break;
      case Level::DEBUG:
        m_logger->debug(evt.message);
        break;
      case Level::TRACE:
        m_logger->trace(evt.message);
        break;
    }
  }
};

std::unique_ptr<ILogger> crapLogger() {
  return std::unique_ptr<ILogger>(new CoutLogger());
}

std::unique_ptr<ILogger> fileLogger(const std::string &filename) {
  try {
    spdlog::flush_every(std::chrono::seconds(1));
    auto file_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
        "file_logger", filename, 1024 * 1024, 3);
    file_logger->set_level(spdlog::level::debug);
    return std::unique_ptr<ILogger>(new SpdLogger(file_logger));

  } catch (const spdlog::spdlog_ex &ex) {
    std::cout << "Log initialization failed : " << ex.what() << ENDL;
    return crapLogger();
  }
}

} // namespace vke::log