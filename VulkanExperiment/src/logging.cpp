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

// #include <move>
#define ENABLE_LOG

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

class CoutSink {
public:
  typedef char char_type;
  typedef boost::iostreams::sink_tag category;

  CoutSink() = default;
  ~CoutSink() = default;

  std::streamsize write(char_type const *s, std::streamsize n) {
    BOOST_LOG_TRIVIAL(info) << std::string(s, n);
    return n;
  }
};

io::stream<io::null_sink> nullOstream((io::null_sink()));

std::ostream &info() {
#if defined(ENABLE_LOG)
  io::stream_buffer<CoutSink> cout_buf((CoutSink()));
  std::ostream whut(&cout_buf);
  return whut;
#else
  return nullOstream;
#endif
}

std::ostream &debug() {
#if defined(ENABLE_LOG)
  io::stream_buffer<CoutSink> cout_buf((CoutSink()));
  std::ostream whut(&cout_buf);
  return whut;
#else
  return nullOstream;
#endif
}

} // namespace vke::log