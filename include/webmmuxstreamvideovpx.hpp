// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMMUXSTREAMVIDEOVPX_HPP
#define WEBMMUXSTREAMVIDEOVPX_HPP

#include <vector>
#include "webmmuxstreamvideo.hpp"
#include "webmmuxmediasampleinterface.hpp"

namespace webmmux
{
class Context;

// Implements MediaSampleInterface to pass sample data to Receive.
class MediaSampleVPx : public MediaSampleInterface
{
public:
  MediaSampleVPx();
  virtual long long GetAbsoluteStartTimeNs() const;
  virtual unsigned long GetBufferLength() const;
  virtual unsigned char* GetBufferPtr() const;

  // This absolute timecode is given in terms of the video timebase. That is
  // to say:
  // absolute time (in sec) = start time * timebase rate / timebase scale
  void SetStartTimecode(const long long start_time);
  void SetTimebaseRate(const long long timebase_rate);
  void SetTimebaseScale(const long long timebase_scale);

  void SetBufferLength(const unsigned long frame_length);
  void SetBufferPtr(unsigned char* frame_ptr);
private:
  long long m_start_time;
  long long m_timebase_rate;   // Defaults to 1.
  long long m_timebase_scale;  // Defaults to 1000.
  unsigned long m_frame_length;
  unsigned char *m_frame_ptr;
};

class StreamVideoVPx : public StreamVideo
{
  StreamVideoVPx(const StreamVideoVPx&);
  StreamVideoVPx& operator=(const StreamVideoVPx&);

public:
  StreamVideoVPx(Context& context, const unsigned short width,
                 const unsigned short height);

  virtual unsigned short GetWidth();
  virtual unsigned short GetHeight();

  int Receive(MediaSampleInterface* pSample);
  int EndOfStream();

protected:
  void WriteTrackCodecID();
  void WriteTrackCodecName();

private:
  unsigned short m_width;
  unsigned short m_height;

  class VPxFrame : public VideoFrame
  {
    VPxFrame(const VPxFrame&);
    VPxFrame& operator=(const VPxFrame&);

  private:
    unsigned long m_timecode;
    unsigned char* m_data;
    unsigned long m_size;

  public:
    explicit VPxFrame(MediaSampleVPx*, StreamVideoVPx*);
    ~VPxFrame();

    bool IsKey() const;
    unsigned long GetTimecode() const;
    unsigned long GetSize() const;
    const unsigned char* GetData() const;
  };

public:
  long GetLastTimecode() const;
};

}  // namespace webmmux

#endif  // WEBMMUXSTREAMVIDEOVPX_HPP
