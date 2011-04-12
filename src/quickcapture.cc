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

// This example illustrates capturing video (no audio support yet) and
// compressing to a webm file.

#include <webmmuxcontext.hpp>
#include <webmmuxebmlio.hpp>
#include <webmmuxstreamaudiovorbis.hpp>
#include <webmmuxstreamvideovpx.hpp>

#include "config.h"
#include "logger.h"
#include "quickcapture.h"

#ifdef WINDOWS
#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#endif

using webmmux::Context;
using webmmux::StreamVideoVPx;
using webmmux::MediaSampleVPx;

// Main entry point into the program.  Captures video until the user presses
// the enter key.
int main(int argc, char *argv[]) {
  LOG("QuickCaptureSample: (-? for help)");
  Config config;
  config.Init(argc, argv);
  QuickCapture quick_capture(config);
  quick_capture.CaptureVideo();
  return 0;
}

QuickCapture::QuickCapture(const Config config)
    : config_(config), webm_file_("sample.webm") {
  // Set up the webm muxer
  webm_context_.m_writing_app = L"libwebm sample_muxer";
  video_capturer_.reset(VideoCapturer::CreateInstance(config_));

  // Create and attach the input video stream.
  // Width and height are hardcoded attributes for this sample.
  // We could pull the WxH from the first keyframe, but normally you get
  // this info from the vpx encoder or parser.  Hardcoded for simplicity.
  video_stream_.reset(new StreamVideoVPx(webm_context_, config_.display_width(),
                                         config_.display_height()));
  webm_context_.SetVideoStream(video_stream_.get());

  AttachOutputStream();

  InitEncoder();
}

void QuickCapture::AttachOutputStream() {
  // SampleFileStream inherits the interface used with the webm context.
  // We open our output file when the SampleFileStream is created.
  if (!webm_file_.IsOpen()) {
    LOG("Failed to open sample.webm as output.\n");
    exit(EXIT_FAILURE);
  }
  // Attach the output stream
  webm_context_.Open(&webm_file_);
}

void QuickCapture::InitEncoder() {
  vpx_codec_enc_cfg_t vpx_encoder_config;
  config_.FillVpxConfiguration(&vpx_encoder_config);

  vpx_codec_enc_init(&encoder_, &vpx_codec_vp8_cx_algo, &vpx_encoder_config, 0);
  vpx_codec_control_(&encoder_, VP8E_SET_CPUUSED, config_.cpu_used());
  vpx_codec_control_(&encoder_, VP8E_SET_STATIC_THRESHOLD,
                     config_.static_threshold());
  vpx_codec_control_(&encoder_, VP8E_SET_ENABLEAUTOALTREF, 0);
}

void QuickCapture::CaptureVideo() {
  video_capturer_->StartCapture();

  vpx_img_alloc(video_capturer_->raw(), IMG_FMT_YV12, config_.display_width(),
                config_.display_height(), 1);

  while (!video_capturer_->Finished()) {
    EncodeAndOutputCapturedFrames();
  }
  CleanUp();
}

void QuickCapture::EncodeAndOutputCapturedFrames() {
  // Make sure we have a frame in our packet store.
  if (video_capturer_->BufferHasFrame()) {
    vpx_codec_encode(&encoder_, video_capturer_->raw(), TimeInBase(),
                     kMillion * 30, config_.encoder_flags(), VPX_DL_REALTIME);

    if (encoder_.err) {
      LOG("Failed to encode frame: %s\n", vpx_codec_error(&encoder_));
      exit(EXIT_FAILURE);
    }

    ProcessPackets();
    video_capturer_->set_buffer_has_frame(false);
  }
}

void QuickCapture::ProcessPackets() {
  const vpx_codec_cx_pkt_t *vp8_packet;
  vpx_codec_iter_t iter = NULL;
  MediaSampleVPx video_media_sample;  // Carries data to the output video
                                      // stream attached to the muxer.
  while ((vp8_packet = vpx_codec_get_cx_data(&encoder_, &iter))) {
    if (vp8_packet->kind == VPX_CODEC_CX_FRAME_PKT) {
      // For this sample, we only consume frame packets.
      if (vp8_packet->kind == VPX_CODEC_CX_FRAME_PKT) {
        // Extract the info and put it in the media sample.
        PopulateVideoMediaSample(*vp8_packet, &video_media_sample);
        // Send the processed sample to the muxer.
        video_stream_->Receive(&video_media_sample);
        LOG("Recieved %s", vp8_packet->data.frame.flags & VPX_FRAME_IS_KEY ?
                           "Keyframe" : "Frame");
      }
    }
  }
}

// Parse data from your vp8 packet into a MediaSampleVPx object.
void QuickCapture::PopulateVideoMediaSample(
    const vpx_codec_cx_pkt_t &vp8_packet,
    MediaSampleVPx *video_media_sample) {
  // Set the timecode in absolute sample number in terms of the timebase.
  video_media_sample->SetStartTimecode(vp8_packet.data.frame.pts);
  video_media_sample->SetTimebaseRate(config_.timebase_numerator());
  video_media_sample->SetTimebaseScale(config_.timebase_denominator());
  video_media_sample->SetBufferLength(
      static_cast<unsigned long>(vp8_packet.data.frame.sz));
  video_media_sample->SetBufferPtr(
      reinterpret_cast<unsigned char*>(vp8_packet.data.frame.buf));
}

void QuickCapture::CleanUp() {
  video_stream_->EndOfStream();
  webm_context_.Close();
  webm_context_.SetVideoStream(0);
  webm_context_.SetAudioStream(0);
  vpx_codec_destroy(&encoder_);
}
