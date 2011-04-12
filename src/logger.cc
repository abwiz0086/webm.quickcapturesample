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

#include <errno.h>
#include <time.h>  // For time, strftime, gmtime.
#include <stdio.h>  // For fprintf, fopen, fclose, etc.

#include "logger.h"

// va_copy portability definitions. Part of va_copy adoption; lifted from
// then-current google3/base/port.h
#ifdef _MSC_VER
// MSVC doesn't have va_copy yet.
// This is believed to work for 32-bit msvc.  This may not work at all for
// other platforms.
// If va_list uses the single-element-array trick, you will probably get
// a compiler error here.
//
#include <stdarg.h>
inline void va_copy(va_list& a, va_list& b) {
  a = b;
}
#endif

void Logger::Log(const char* message, ...) {
  va_list vl;
  va_start(vl, message);
  LogV(message, vl);
  va_end(vl);
}

void Logger::LogV(const char* message, va_list list) {
  string message_with_params;
  StringAppendV(&message_with_params, message, list);
  fprintf(stderr, "%s\n", message_with_params.c_str());
}

void Logger::StringAppendV(string* dst, const char* format, va_list ap) {
  // First try with a small fixed size buffer
  char space[1024];

  // It's possible for methods that use a va_list to invalidate
  // the data in it upon use.  The fix is to make a copy
  // of the structure before using it and use that copy instead.
  va_list backup_ap;
  va_copy(backup_ap, ap);
  int result = vsnprintf(space, sizeof(space), format, backup_ap);
  va_end(backup_ap);

  if ((result >= 0) && ((unsigned int) result < sizeof(space))) {
    // It fit
    dst->append(space, result);
    return;
  }

  // Repeatedly increase buffer size until it fits
  int length = sizeof(space);
  while (true) {
    if (result < 0) {
      // Older behavior: just try doubling the buffer size
      length *= 2;
    } else {
      // We need exactly "result+1" characters
      length = result+1;
    }
    char* buf = new char[length];

    // Restore the va_list before we use it again
    va_copy(backup_ap, ap);
    int result = vsnprintf(buf, length, format, backup_ap);
    va_end(backup_ap);

    if ((result >= 0) && (result < length)) {
      // It fit
      dst->append(buf, result);
      delete[] buf;
      return;
    }
    delete[] buf;
  }
}
