// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMMUXSTREAMAUDIO_HPP
#define WEBMMUXSTREAMAUDIO_HPP

#include <list>
#include "webmmuxstream.hpp"

namespace webmmux
{
class StreamAudio : public Stream
{
  StreamAudio(const StreamAudio&);
  StreamAudio& operator=(const StreamAudio&);

protected:
  explicit StreamAudio(Context& ctx);
  ~StreamAudio();

  void WriteTrackType();
  void WriteTrackSettings();

  virtual unsigned long GetSamplesPerSec() const = 0;
  virtual unsigned char GetChannels() const = 0;

public:
  void Flush();
  bool Wait() const;

  class AudioFrame : public Frame
  {
    AudioFrame(const AudioFrame&);
    AudioFrame& operator=(const AudioFrame&);

  protected:
    AudioFrame();

  public:
    bool IsKey() const;
  };

  typedef std::list<AudioFrame*> frames_t;
  frames_t& GetFrames();

private:
  frames_t m_frames;
};

}  // namespace webmmux

#endif  // WEBMMUXSTREAMAUDIO_HPP
