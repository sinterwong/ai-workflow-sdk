#ifndef _MY_LOGGER_HPP__
#define _MY_LOGGER_HPP__

#include <atomic>
#include <glog/logging.h>
#include <mutex>
#include <string>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

class Logger {
public:
  struct LogConfig {
    std::string logPath;
    std::string appName = "App";
    int logLevel = google::INFO;
    bool enableConsole = true;
    bool enableColor = true;
  };

  static Logger *instance();
  void initialize(const LogConfig &config);
  void shutdown();

  const char *getColorPrefix(google::LogSeverity severity) const;
  const char *getColorSuffix() const;

  bool isInitialized() const;
  const LogConfig &getConfig() const { return config_; }

private:
  Logger();
  ~Logger();

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  std::mutex mutex_;
  std::atomic<bool> isInitialized_;
  LogConfig config_;
};

class MyLogMessage {
public:
  MyLogMessage(const char *file, int line, google::LogSeverity severity)
      : glog_message_(file, line, severity) {
    glog_message_.stream() << Logger::instance()->getColorPrefix(severity);
  }

  ~MyLogMessage() {
    glog_message_.stream() << Logger::instance()->getColorSuffix();
  }

  std::ostream &stream() { return glog_message_.stream(); }

private:
  google::LogMessage glog_message_;
};

#define LOG_STREAM(severity)                                                   \
  MyLogMessage(__FILE__, __LINE__, google::severity).stream()

#define LOG_INFOS LOG_STREAM(INFO)
#define LOG_WARNINGS LOG_STREAM(WARNING)
#define LOG_ERRORS LOG_STREAM(ERROR)
#define LOG_FATALS LOG_STREAM(FATAL)
#endif // _MY_LOGGER_HPP__
