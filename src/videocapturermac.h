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

// Captures video on a mac machine, and stores it in raw_->img_data in
// YVY12 format.

#ifndef QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERMAC__
#define QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERMAC__

#include <vidcap/vidcap.h>

#include "config.h"
#include "videocapturer.h"

class VideoCapturerMac : public VideoCapturer {
 public:
   VideoCapturerMac(Config config) : VideoCapturer(config) {};
  ~VideoCapturerMac() {}

  virtual bool BufferHasFrame();
  virtual void StartCapture();

 private:
  struct my_source_context {
    vidcap_src *src;
    char name[VIDCAP_NAME_LENGTH];
  };

  static int UserCaptureCallback(vidcap_src *src,
  void *user_data, struct vidcap_capture_info *cap_info);
};

#endif // QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERMAC__
