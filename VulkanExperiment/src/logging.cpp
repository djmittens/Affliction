#include "VulkanExperiment/logging.hpp"
#include <boost/format/format_implementation.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/log/trivial.hpp>
#include <cstdarg>
#include <cstdio>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace vke::log {
namespace io = boost::iostreams;

// class Logger_Sink {
// public:
//   typedef char char_type;
//   typedef boost::iostreams::sink_tag category;

//   Logger_Sink() = default;

//   virtual ~Logger_Sink() = default;
//   virtual std::streamsize write(char_type const *s, std::streamsize n) = 0;
// };

// class CoutSink {
// public:
//   typedef char char_type;
//   typedef boost::iostreams::sink_tag category;

//   CoutSink() = default;
//   ~CoutSink() = default;

//   std::streamsize write(char_type const *s, std::streamsize n) { return n; }
// };

// io::stream<io::null_sink> nullOstream((io::null_sink()));

// std::ostream &info() {
// #if defined(ENABLE_LOG)
//   io::stream_buffer<CoutSink> cout_buf((CoutSink()));
//   std::ostream whut(&cout_buf);
//   return whut;
// #else
//   return nullOstream;
// #endif
// }

class BoostLogger : public ILogger {

public:
  ~BoostLogger() = default;
  virtual void trace(LogEvent evt) {}
  virtual LogLevel logLevel() override { return LogLevel::TRACE; }
  virtual void info(LogEvent evt) { BOOST_LOG_TRIVIAL(info) << evt.message; }
  virtual void debug(LogEvent evt) { BOOST_LOG_TRIVIAL(info) << evt.message; }
  virtual void warn(LogEvent evt) {}
  virtual void error(LogEvent evt) { BOOST_LOG_TRIVIAL(error) << evt.message; }
};

std::unique_ptr<ILogger> crapLogger() {
  return std::make_unique<ILogger>(BoostLogger());
}

} // namespace vke::log