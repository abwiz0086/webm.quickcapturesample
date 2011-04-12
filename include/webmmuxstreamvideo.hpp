// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMMUXSTREAMVIDEO_HPP
#define WEBMMUXSTREAMVIDEO_HPP

#include <climits>
#include <list>
#include "webmmuxstream.hpp"

namespace webmmux
{
class StreamVideo : public Stream
{
  explicit StreamVideo(const StreamVideo&);
  StreamVideo& operator=(const StreamVideo&);

protected:
  explicit StreamVideo(Context&);
  ~StreamVideo();

  virtual unsigned short GetWidth() = 0;
  virtual unsigned short GetHeight() = 0;

  void WriteTrackType();
  void WriteTrackSettings();

public:
  void Flush();
  bool Wait() const;

  class VideoFrame : public Frame
  {
    VideoFrame(const VideoFrame&);
    VideoFrame& operator=(const VideoFrame&);

  protected:
    VideoFrame();
  };

  virtual long GetLastTimecode() const = 0;

  typedef std::list<VideoFrame*> frames_t;
  frames_t& GetFrames();
  frames_t& GetKeyFrames();

protected:
  frames_t m_vframes;
  frames_t m_rframes;
};

}  // namespace webmmux

#endif  // WEBMMUXSTREAMVIDEO_HPP
