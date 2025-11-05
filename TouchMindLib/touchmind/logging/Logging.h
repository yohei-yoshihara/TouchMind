#ifndef TOUCHMIND_LOGGING_LOGGING_H_
#define TOUCHMIND_LOGGING_LOGGING_H_

#if defined(DEBUG) || defined(_DEBUG)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#define SPDLOG_HEADER_ONLY
#define SPDLOG_WCHAR_FILENAMES
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace touchmind {
namespace logging {
extern void InitLogger(const wchar_t *logFileName);
}
}  // namespace touchmind

#endif // TOUCHMIND_LOGGING_LOGGING_H_