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

// Captures video (no audio support yet) and compresses to a webm file.

#include "samplefilestream.h"
#include "videocapturer.h"

class QuickCapture {
 public:
  QuickCapture(const Config config);

  // Begins the process of capturing streaming video and sending to a
  // sample.webm file.  User should press enter to end the streaming.
  void CaptureVideo();

 private:
  void AttachOutputStream();
  void InitEncoder();
  // Encodes any frames captured thus far and sends them to the output stream.
  void EncodeAndOutputCapturedFrames();
  // Processes encoded packets & sends them to the output stream.
  void ProcessPackets();
  void PopulateVideoMediaSample(const vpx_codec_cx_pkt_t &vp8_packet,
                                webmmux::MediaSampleVPx *video_media_sample);
  void CleanUp();
  long long TimeInBase() {
    long long time_in_base = (long long)(video_capturer_->buffer_seconds() *
        config_.timebase_denominator() + .5);
    LOG_ASSERT(time_in_base > 0);
    return time_in_base;
  }


  // Contains configuration variables, some which are adjustable by passing in
  // specific flags on the command line.
  Config config_;
  // For now, hardcoded as a file named 'sample.webm'.
  SampleFileStream webm_file_;
  // Handles grabbing the raw data in YV12 format from the video device.
  auto_ptr<VideoCapturer> video_capturer_;
  auto_ptr<webmmux::StreamVideoVPx> video_stream_;
  webmmux::Context webm_context_;
  // Handles encoding YV12 format into webm format.
  vpx_codec_ctx_t encoder_;
};
