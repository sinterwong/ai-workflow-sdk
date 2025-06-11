#include "logger.hpp"
#include <filesystem>

Logger *Logger::instance() {
  static Logger instance;
  return &instance;
}

Logger::Logger() : isInitialized_(false) {}

Logger::~Logger() { shutdown(); }

void Logger::initialize(const LogConfig &config) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (isInitialized_.load()) {
    LOG(INFO) << "Logger already initialized";
    return;
  }

  config_ = config;

  google::InitGoogleLogging(config.appName.c_str());
  google::SetLogFilenameExtension(".log");

  std::string logDirectory = config.logPath;
  if (!logDirectory.empty() && logDirectory.back() != '/' &&
      logDirectory.back() != '\\') {
    logDirectory += '/';
  }

  if (!logDirectory.empty()) {
    std::filesystem::create_directories(std::filesystem::path(logDirectory));
    google::SetLogDestination(google::INFO, logDirectory.c_str());
    google::SetLogDestination(google::WARNING, logDirectory.c_str());
    google::SetLogDestination(google::ERROR, logDirectory.c_str());
    google::SetLogDestination(google::FATAL, logDirectory.c_str());
  }

  FLAGS_minloglevel = config.logLevel;

  if (config.enableConsole) {
    auto logSeverity = static_cast<google::LogSeverity>(config.logLevel);
    google::SetStderrLogging(logSeverity);
  }

  LOG(INFO) << "Logger initialized successfully";
  isInitialized_.store(true);
}

void Logger::shutdown() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (isInitialized_.load()) {
    google::ShutdownGoogleLogging();
    isInitialized_.store(false);
  }
}

const char *Logger::getColorPrefix(google::LogSeverity severity) const {
  if (!config_.enableColor) {
    return "";
  }

  switch (severity) {
  case google::INFO:
    return ANSI_COLOR_GREEN;
  case google::WARNING:
    return ANSI_COLOR_YELLOW;
  case google::ERROR:
    return ANSI_COLOR_RED;
  case google::FATAL:
    return ANSI_COLOR_RED;
  default:
    return "";
  }
}

const char *Logger::getColorSuffix() const {
  return config_.enableColor ? ANSI_COLOR_RESET : "";
}

bool Logger::isInitialized() const { return isInitialized_.load(); }
