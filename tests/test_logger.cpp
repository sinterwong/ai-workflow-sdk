#include "logger/logger.hpp"
#include <gtest/gtest.h>

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    static std::once_flag initFlag;
    static std::once_flag loggerInitFlag;
    std::call_once(loggerInitFlag, [&]() {
      Logger::LogConfig logConfig;
      logConfig.appName = "Test-Unit";
      logConfig.logPath = "./logs";
      logConfig.logLevel = 0;
      logConfig.enableColor = true;
      Logger::instance()->initialize(logConfig);
    });
  }
  void TearDown() override {}

  Logger::LogConfig config;
};

TEST_F(LoggerTest, Normal) {
  LOG_INFOS << "(INFO) Hello World!";
  LOG_WARNINGS << "(WARN) Hello World!";
  LOG_ERRORS << "(ERROR) Hello World!";
  // LOG_FATALS << "(FATAL) Hello World!";
}
