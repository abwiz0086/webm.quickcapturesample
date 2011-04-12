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

#include "videocapturermac.h"
#include "planeconverter.h"
#include "logger.h"

bool VideoCapturerMac::BufferHasFrame() {
  if (buffer_has_frame_) {
    buffer_seconds_ = TimeInSeconds();
  }
  return buffer_has_frame_;
}

int VideoCapturerMac::UserCaptureCallback(vidcap_src *src,
    void *user_data, struct vidcap_capture_info *cap_info) {
  VideoCapturerMac* capturer = (VideoCapturerMac*)user_data;
  if (capturer->UserEnteredKey()) {
    capturer->finished_ = true;
    return 0;
  }
  PlaneConverter::YuyToYv12(capturer->raw_->img_data,
                            (char*)cap_info->video_data,
                            capturer->raw_->d_w,  capturer->raw_->d_h);
  capturer->buffer_has_frame_ = true;
  return 0;
}

void VideoCapturerMac::StartCapture() {
  vidcap_state *vc = vidcap_initialize();
  FailOnZero((int)vc, "Failed vidcap_initialize");

  vidcap_sapi *sapi = vidcap_sapi_acquire(vc, 0);
  FailOnZero((int)sapi, "Failed vidcap_sapi_acquire");

  struct vidcap_sapi_info sapi_info;
  FailOnNonZero(vidcap_sapi_info_get(sapi, &sapi_info),
                "Failed vidcap_sapi_info_get");

  int src_list_len = vidcap_src_list_update(sapi);
  FailOnNegative(src_list_len, "Failed vidcap_src_list_update");
  FailOnZero(src_list_len, "No sources available");

  struct vidcap_src_info *src_list =
      (struct vidcap_src_info *)calloc(src_list_len,
                                       sizeof(struct vidcap_src_info));
  FailOnZero((int)src_list, "Failed allocating memory for src_list");

  FailOnNonZero(vidcap_src_list_get(sapi, src_list_len, src_list),
                "Failed on vidcap_src_list_get");

  struct my_source_context *ctx_list =
      (my_source_context *)calloc(src_list_len, sizeof(*ctx_list));
  FailOnZero((int)ctx_list, "Failed on allocating ctx list");

  for (int i = 0; i < src_list_len; ++i) {
    struct vidcap_fmt_info fmt_info;
    ctx_list[i].src = vidcap_src_acquire(sapi, &src_list[i]);
    fmt_info.width = config_.display_width();
    fmt_info.height = config_.display_height();
    fmt_info.fps_numerator = config_.capture_frame_rate();
    fmt_info.fps_denominator = 1;
    fmt_info.fourcc = VIDCAP_FOURCC_YUY2;

    FailOnNonZero(vidcap_format_bind(ctx_list[i].src, &fmt_info),
                  "vidcap_format_bind Failed");
    FailOnNonZero(vidcap_format_info_get(ctx_list[i].src, &fmt_info),
                  "vidcap_format_info_get");

    sprintf(ctx_list[i].name, "source %d", i);

    FailOnNonZero(vidcap_src_capture_start(ctx_list[i].src,
                                           this->UserCaptureCallback, this),
                  "Failed on vidcap_src_capture_start");
  }

  free(src_list);
}
