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

// An abstract base class for capturing video streams. Subclassed for specifics
// to windows, mac and linux.

#ifndef QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURER__
#define QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURER__

#include <memory>
#include <string>

#include "config.h"

#ifndef WINDOWS
// For time calculations in mac & linux
#include <sys/time.h>
#include <cstdlib>
#endif

class VideoCapturer {
 public:
  VideoCapturer(Config config);
  virtual ~VideoCapturer() {};

  // no-op by default.
  virtual void Init() {}
  virtual bool BufferHasFrame() = 0;

  void set_buffer_has_frame(bool val) {
    buffer_has_frame_ = val;
  };
  double buffer_seconds() { return buffer_seconds_; }

  virtual void Finish() { finish_ = true; }
  virtual bool Finished() { return finished_; }

  virtual void StartCapture() = 0;
  virtual vpx_image_t* raw() { return raw_.get(); }

  static VideoCapturer* CreateInstance(Config config);
  double TimeInSeconds();

protected:
  // no-op by default.
  virtual void CleanUp() {}
  void MaybeCleanUpAndFail(bool should_fail, std::string error_message);
  void FailOnNegative(int result, std::string message);
  void FailOnNonZero(int result, std::string message);
  void FailOnZero(int result, std::string message);
  bool UserEnteredKey();

  Config config_;
  double buffer_seconds_;
  bool buffer_has_frame_;
  bool finish_;
  bool finished_;
  std::auto_ptr<vpx_image_t> raw_;
};

#endif // QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURER__
