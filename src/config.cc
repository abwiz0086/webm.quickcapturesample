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

#include <stdio.h> // for printf
#include <stdlib.h> // for _stricmp and exit
#ifndef WINDOWS
#include <strings.h> // for strcasecmp
#endif

#include "config.h"
#include "logger.h"

const int kRequestRecoveryNormal = 0;
// TODO: Allow request recovery flags to be specified on the command line. For
// now only normal is used.
// const int kRequestRecoveryKey = 1;
// const int kRequestRecoveryGold = 2;
// const int kRequestRecoveryAltRef = 3;

// TODO: Likewise, allow a few more of these default values to be configurable
// via command line.
const int kDefaultTimebaseNumerator = 1;
const int kDefaultTimebaseDenominator = kBillion;
const int kDefaultDisplayWidth = 640;
const int kDefaultDisplayHeight = 480;
const int kDefaultCaptureFrameRate = 30;
const int kDefaultVideoBitrate = 30;
const int kDefaultCpuUsed = 8;
const int kDefaultStaticThreshold = 1200;
const int kDefaultRequestRecovery = kRequestRecoveryNormal;

Config::Config()
    : timebase_denominator_(kDefaultTimebaseDenominator),
      timebase_numerator_(kDefaultTimebaseNumerator),
      display_width_(kDefaultDisplayWidth),
      display_height_(kDefaultDisplayHeight),
      capture_frame_rate_(kDefaultCaptureFrameRate),
      video_bitrate_(kDefaultVideoBitrate),
      cpu_used_(kDefaultCpuUsed), static_threshold_(kDefaultStaticThreshold),
      request_recovery_(kDefaultRequestRecovery) { }

void Config::Init(int argc, char *argv[]) {
  bool success = false;
  while ((argc -= 2) > 0) {
    success = argv[argc][0] == '-';
    if (success) {
      char flag_name[80];
      success = sscanf(argv[argc], "-%s", flag_name) == 1;
      int new_value;
      char bad_char;
      success = success &&
                sscanf(argv[argc + 1], "%d%c", &new_value, &bad_char) == 1;
      if (success) {
        if (StringsEqual(flag_name, "w") || StringsEqual(flag_name, "width")) {
          display_width_ = new_value;
        } else if (StringsEqual(flag_name, "h") ||
                   StringsEqual(flag_name, "height")) {
          display_height_ = new_value;
        } else if (StringsEqual(flag_name, "f") ||
                   StringsEqual(flag_name, "framerate")) {
          capture_frame_rate_ = new_value;
        } else if (StringsEqual(flag_name, "b") ||
                   StringsEqual(flag_name, "bitrate")) {
          video_bitrate_ = new_value;
        } else {
          success = false;
        }
      }
    }
    if (!success) {
      LOG("CaptureSample: \n"
          "========================: \n"
          "Captures streaming video and outputs to a sample.webm file.\n\n"
          "-w [640]  request capture width \n"
          "-h [480]  request capture height \n"
          "-f [30]   request capture frame rate\n"
          "-b [300]  video_bitrate = ato\n"
          "\n");
      exit(0);
    }
  }
  PrintConfiguration();
}

bool Config::StringsEqual(char* first, char* second) {
#ifdef WINDOWS
  return !_stricmp(first, second);
#else
  return !strcasecmp(first, second);
#endif
}

void Config::PrintConfiguration() {
  LOG("%dx%d %dfps, %dkbps\n", display_width_,
      display_height_, capture_frame_rate_, video_bitrate_);
}

void Config::FillVpxConfiguration(vpx_codec_enc_cfg_t* configuration){
  vpx_codec_enc_config_default(&vpx_codec_vp8_cx_algo, configuration, 0);
  configuration->rc_target_bitrate = video_bitrate_;
  configuration->g_w = display_width_;
  configuration->g_h = display_height_;
  configuration->g_timebase.num = timebase_numerator_;
  configuration->g_timebase.den = timebase_denominator_;
  configuration->rc_end_usage = VPX_CBR;
  configuration->g_pass = VPX_RC_ONE_PASS;
  configuration->g_lag_in_frames = 0;
  configuration->rc_min_quantizer = 20;
  configuration->rc_max_quantizer = 50;
  configuration->rc_dropframe_thresh = 1;
  configuration->rc_buf_optimal_sz = 1000;
  configuration->rc_buf_initial_sz = 1000;
  configuration->rc_buf_sz = 1000;
  configuration->g_error_resilient = 1;
  configuration->kf_mode = VPX_KF_DISABLED;
  configuration->kf_max_dist = 999999;
  configuration->g_threads = 1;
}

int Config::encoder_flags() {
  const unsigned int kRecoveryFlags[] = {
    0, //   NORMAL,
    VPX_EFLAG_FORCE_KF, //   KEY,
    VP8_EFLAG_FORCE_GF | VP8_EFLAG_NO_UPD_ARF |
    VP8_EFLAG_NO_REF_LAST | VP8_EFLAG_NO_REF_ARF, //   GOLD = 2,
    VP8_EFLAG_FORCE_ARF | VP8_EFLAG_NO_UPD_GF |
    VP8_EFLAG_NO_REF_LAST | VP8_EFLAG_NO_REF_GF  //   ALTREF = 3
  };
  return kRecoveryFlags[request_recovery_];
}
