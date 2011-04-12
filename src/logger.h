/*
 *  Copyright (c) 2011 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *  Author: sgammon@google.com (Stacey Gammon)
 */

// Logger: Contains macros for printing messages to the console.

#ifndef QUICKCAPTURESAMPLE_SRC_LOGGER_H__
#define QUICKCAPTURESAMPLE_SRC_LOGGER_H__

#include <assert.h>
#include <stdarg.h>  // For va_list, va_start, etc.
#include <string>

using namespace std;

#define LOG Logger::Log
#define LOG_LOCATION() Logger::Log( \
  "%s:%s:%d",__FILE__, __FUNCTION__, __LINE__)

#define ALWAYS_LOG_LOCATION() Logger::Log( \
  "%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)

// Uses log level none to unconditionally log some info messages before
// asserting if condition is false.
#define LOG_ASSERT_INFO(condition, message) \
    do { \
      if (!(condition)) { \
        Logger::Log("ASSERTION \"%s\"FAILED AT:", #condition); \
        ALWAYS_LOG_LOCATION(); \
        Logger::Log(message); \
      } \
    } while (0)

// Uses log level none to unconditionally log some info messages before
// asserting if condition is false.
#define LOG_ASSERT(condition) \
    do { \
      if (!(condition)) { \
        Logger::Log("ASSERTION \"%s\" FAILED AT:", #condition); \
        ALWAYS_LOG_LOCATION(); \
        assert(false); \
      } \
    } while (0)

class Logger {
 public:
  // Logs a message with potential parameters as long as the instance's
  // level is less than or equal to the designated level.
  static void Log(const char* message, ...);
  // Logs a message given a va_list with parameters
  static void LogV(const char* message, va_list list);

 private:
  static void StringAppendV(string* dst, const char* format, va_list ap);
};

#endif // QUICKCAPTURESAMPLE_SRC_LOGGER_H__
