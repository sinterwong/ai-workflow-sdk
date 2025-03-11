#ifndef _MY_LOGGER_HPP__
#define _MY_LOGGER_HPP__

#include <atomic>
#include <glog/logging.h>
#include <mutex>
#include <sstream>
#include <string>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

class LogStream;

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

  void info(const std::string &message);
  void warning(const std::string &message);
  void error(const std::string &message);
  void fatal(const std::string &message);

  LogStream infoStream();
  LogStream warningStream();
  LogStream errorStream();
  LogStream fatalStream();

  static void logInfo(const std::string &message);
  static void logWarning(const std::string &message);
  static void logError(const std::string &message);
  static void logFatal(const std::string &message);

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

class LogStream {
public:
  LogStream(google::LogSeverity severity, Logger *logger);
  ~LogStream();

  template <typename T> LogStream &operator<<(const T &value) {
    stream_ << value;
    return *this;
  }

private:
  std::ostringstream stream_;
  google::LogSeverity severity_;
  Logger *logger_;
};

#define LOG_INFO(message) Logger::logInfo(message)
#define LOG_WARNING(message) Logger::logWarning(message)
#define LOG_ERROR(message) Logger::logError(message)
#define LOG_FATAL(message) Logger::logFatal(message)

#define LOG_INFOS Logger::instance()->infoStream()
#define LOG_WARNINGS Logger::instance()->warningStream()
#define LOG_ERRORS Logger::instance()->errorStream()
#define LOG_FATALS Logger::instance()->fatalStream()

#endif // _MY_LOGGER_HPP__