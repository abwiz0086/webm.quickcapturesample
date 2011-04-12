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

// Captures video using dshow on a linux/unix machines, and releases it in YVY12
// format.

#ifndef QUICKCAPTURESAMPLE_SRC_VIDEOCAPTUREROTHER_H__
#define QUICKCAPTURESAMPLE_SRC_VIDEOCAPTUREROTHER_H__

#include "config.h"
#include "uvcvideo.h"
#include "videocapturer.h"

const int kMaxBufferCount = 4;
const v4l2_buf_type kVideoCaptureType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
const v4l2_memory kVideoMemoryMMap = V4L2_MEMORY_MMAP;

class VideoCapturerOther : public VideoCapturer {
 public:

  VideoCapturerOther(Config config) : VideoCapturer(config) { };

  virtual void StartCapture();
  virtual bool BufferHasFrame();

 private:
  virtual void CleanUp();
  void InitFormat();
  void InitRequestBuffers();
  void SetStreamParameters();
  void SetCapabilities();
  void MapAndQueueBuffers();

  void *mem_[kMaxBufferCount];
  int fd_;
};

#endif // QUICKCAPTURESAMPLE_SRC_VIDEOCAPTUREROTHER_H__
