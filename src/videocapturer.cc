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

#include "logger.h"
#include "videocapturer.h"
#ifdef WINDOWS
#include <conio.h> // for _khibit
#include "videocapturerwin.h"
#elif MACOSX
#include "videocapturermac.h"
#else
#include "videocapturerother.h"
#endif

VideoCapturer::VideoCapturer(Config config)
    : config_(config), buffer_seconds_(0), buffer_has_frame_(false),
      finish_(false), finished_(false) {
  raw_.reset(new vpx_image_t());
};

VideoCapturer* VideoCapturer::CreateInstance(Config config) {
  VideoCapturer* capturer;
#ifdef WINDOWS
  capturer = new VideoCapturerWin(config);
#elif MACOSX
  capturer = new VideoCapturerMac(config);
#else
  capturer = new VideoCapturerOther(config);
#endif
  capturer->Init();
  return capturer;
}

double VideoCapturer::TimeInSeconds() {
#ifdef WINDOWS
  LOG_ASSERT_INFO(false, "TimeInSeconds Undefined");
  return 0;
#else
  static suseconds_t start_microseconds_ = 0;
  struct timeval now;

  // Check if the start time has been initialized yet.
  if (!start_microseconds_) {
    gettimeofday(&now, NULL);
    start_microseconds_ = (now.tv_sec * 1000000) + (now.tv_usec) - 1000000;
  }

  double time_in_seconds;
  gettimeofday(&now, NULL);
  suseconds_t time_in_microseconds = (now.tv_sec * 1000000) + now.tv_usec;

  suseconds_t diff = (time_in_microseconds - start_microseconds_);
  time_in_seconds = diff / 1000000.00;
  return time_in_seconds;
#endif
}

void VideoCapturer::FailOnNonZero(int result, string message) {
  if (result) {
    MaybeCleanUpAndFail(true, message);
  }
}

void VideoCapturer::FailOnZero(int result, string message) {
  if (!result) {
    MaybeCleanUpAndFail(true, message);
  }
}

void VideoCapturer::FailOnNegative(int result, string message) {
  if (result < 0) {
    MaybeCleanUpAndFail(true, message);
  }
}

void VideoCapturer::MaybeCleanUpAndFail(bool should_fail,
                                        string error_message) {
  if (should_fail) {
    LOG("Failed while %s and error code '%d'", error_message.c_str(), errno);
    CleanUp();
    exit(EXIT_FAILURE);
  }
}

bool VideoCapturer::UserEnteredKey() {
#ifdef WINDOWS
  return (_kbhit() != 0);
#else
  struct timeval tv;
  fd_set read_fd;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&read_fd);
  FD_SET(0, &read_fd);


  if (select(1, &read_fd, NULL, NULL, &tv) == -1) {
    return false;
  }

  if (FD_ISSET(0, &read_fd)) {
    return true;
  }

  return false;
#endif
}
