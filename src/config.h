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

 // Holds configuration details for capturing & encoding/decoding video.

#ifndef QUICKCAPTURESAMPLE_SRC_CONFIG_H__
#define QUICKCAPTURESAMPLE_SRC_CONFIG_H__

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>

const int kBillion = 1000000000;
const int kMillion = 1000000;

class Config {
 public:
  Config();

  // Initialize from command line flags
  void Init(int argc, char* argv[]);

  int timebase_denominator() { return timebase_denominator_; }
  int timebase_numerator() { return timebase_numerator_; }
  int display_width() { return display_width_; }
  int display_height() { return display_height_; }
  int capture_frame_rate() { return capture_frame_rate_; }
  int cpu_used() { return cpu_used_; }
  int static_threshold() { return static_threshold_; }
  int encoder_flags();

  void FillVpxConfiguration(vpx_codec_enc_cfg_t* configuration);

 private:
  void PrintConfiguration();
  bool StringsEqual(char* first, char* second);

  int timebase_denominator_;
  int timebase_numerator_;
  int display_width_;
  int display_height_;
  int capture_frame_rate_;
  int video_bitrate_;
  int cpu_used_;
  int static_threshold_;
  int request_recovery_;
};

#endif // QUICKCAPTURESAMPLE_SRC_CONFIG_H__
