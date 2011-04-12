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
#include <fcntl.h> // for O_RDWR and O_NONBLOC
#include <string.h> // for memset
#include <sys/ioctl.h>
#include <sys/mman.h>  // for mmap

#include "logger.h"
#include "planeconverter.h"
#include "videocapturerother.h"

void VideoCapturerOther::StartCapture() {
  FailOnNegative(fd_ = open("/dev/video0", O_RDWR | O_NONBLOCK),
                 "Failed to open video file descriptor");

  SetCapabilities();
  InitFormat();
  SetStreamParameters();
  InitRequestBuffers();

  MapAndQueueBuffers();

  // Start streaming.
  FailOnNegative(ioctl(fd_, VIDIOC_STREAMON, &kVideoCaptureType),
                 "Failed on ioctl with VIDIOC_STREAMON");
}

void VideoCapturerOther::InitFormat() {
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(struct v4l2_format));
  fmt.type = kVideoCaptureType;
  fmt.fmt.pix.width = config_.display_width();
  fmt.fmt.pix.height = config_.display_height();
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_ANY;

  FailOnNegative(ioctl(fd_, VIDIOC_S_FMT, &fmt),
                 "ioctl VIDIOC_S_FMT failed");
}

void VideoCapturerOther::InitRequestBuffers() {
  struct v4l2_requestbuffers rb;
  memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
  rb.count = kMaxBufferCount;
  rb.type = kVideoCaptureType;
  rb.memory = kVideoMemoryMMap;

  FailOnNonZero(ioctl(fd_, VIDIOC_REQBUFS, &rb),
                "Initializing request buffers failed.");
}

void VideoCapturerOther::SetCapabilities() {
  struct v4l2_capability capability;
  memset(&capability, 0, sizeof(struct v4l2_capability));

  FailOnNegative(ioctl(fd_, VIDIOC_QUERYCAP, &capability),
                 "ioctl with VIDIOC_QUERYCAP failed");
}

void VideoCapturerOther::SetStreamParameters() {
  struct v4l2_streamparm setfps;

  memset(&setfps, 0, sizeof(struct v4l2_streamparm));
  setfps.type = kVideoCaptureType;
  setfps.parm.capture.timeperframe.numerator = 1;
  setfps.parm.capture.timeperframe.denominator = config_.capture_frame_rate();

  FailOnNonZero(ioctl(fd_, VIDIOC_S_PARM, &setfps),
                "ioctl with VIDIOC_S_PARAM failed");
}

void VideoCapturerOther::MapAndQueueBuffers() {
  struct v4l2_buffer buf;
  for (int i = 0; i < kMaxBufferCount; i++) {
    memset(&buf, 0, sizeof(struct v4l2_buffer));
    buf.index = i;
    buf.type = kVideoCaptureType;
    buf.memory = kVideoMemoryMMap;

    FailOnNegative(ioctl(fd_, VIDIOC_QUERYBUF, &buf),
                   "ioctl failed on VIDIOC_QUERYBUF");

    mem_[i] = mmap(0 , buf.length, PROT_READ, MAP_SHARED, fd_, buf.m.offset);
    MaybeCleanUpAndFail(mem_[i] == MAP_FAILED, "Failed while mapping buffers");
    FailOnNegative(ioctl(fd_, VIDIOC_QBUF, &buf),
                   "ioctl with VIDIOC_QBUF failed");
  }
}

bool VideoCapturerOther::BufferHasFrame() {
  if (UserEnteredKey()) {
    CleanUp();
    return false;
  }
  struct v4l2_buffer buf;
  buf.type = kVideoCaptureType;
  buf.memory = kVideoMemoryMMap;

  // Get a frame if we can.
  if (ioctl(fd_, VIDIOC_DQBUF, &buf) < 0) {
    MaybeCleanUpAndFail(errno != EAGAIN, "Failed to grab buffer");
    return false;
  }

  if (buf.bytesused > 0) {
    PlaneConverter::YuyToYv12(raw_->img_data, (char *) mem_[buf.index],
                              raw_->d_w, raw_->d_h);
  }

  // A temporary and very hacky way of getting around a bug where
  // ioctl(fd_, VIDIOC_QBUF, &buf) randomly returns error EINVAL.
  // TODO: fix
  usleep(100000);

  MaybeCleanUpAndFail(ioctl(fd_, VIDIOC_QBUF, &buf) < 0,
                      "Failed putting buffer back");

  buffer_seconds_ = TimeInSeconds();
  return buffer_seconds_ > 0;
}

void VideoCapturerOther::CleanUp() {
  if (ioctl(fd_, VIDIOC_STREAMOFF, &kVideoCaptureType)) {
    LOG("Turning stream off failed.");
  }
  close(fd_);
  finished_ = true;
}
