// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMMUXSTREAMAUDIOVORBIS_HPP
#define WEBMMUXSTREAMAUDIOVORBIS_HPP

#include <vector>
#include "webmmuxstreamaudio.hpp"
#include "webmmuxmediasampleinterface.hpp"

namespace webmmux
{
class Context;

// Implements a Vorbis-specific MediaSampleInterface to pass sample data to
// StreamAudioVorbis::Receive.
class MediaSampleVorbis : public MediaSampleInterface
{
public:
  MediaSampleVorbis();
  virtual long long GetAbsoluteStartTimeNs() const;
  virtual unsigned long GetBufferLength() const;
  virtual unsigned char* GetBufferPtr() const;

  // These absolute timecodes are given in terms of the audio sample rate.
  // That is to say: absolute time (in sec) = time / sample rate (in hz).
  long long GetStartTimecode() const;
  // The Vorbis audio media sample also needs a stop time.
  long long GetStopTimecode() const;

  // Set the samples per second of the audio stream
  void SetSampleRateHz(const long long sample_rate);
  void SetStartTimecode(const long long start_time);
  void SetStopTimecode(const long long stop_time);
  void SetBufferLength(const unsigned long packet_length);
  void SetBufferPtr(unsigned char* packet_ptr);
private:
  long long m_sample_rate;  // defaults to 1.
  long long m_start_time;
  long long m_stop_time;
  unsigned long m_packet_length;
  unsigned char *m_packet_ptr;
};

class StreamAudioVorbis : public StreamAudio
{
  StreamAudioVorbis(const StreamAudioVorbis&);
  StreamAudioVorbis& operator=(const StreamAudioVorbis&);

protected:
  virtual void Final();  // grant last wishes

  unsigned long GetSamplesPerSec() const;
  unsigned char GetChannels() const;

  void WriteTrackCodecID();
  void WriteTrackCodecName();
  void WriteTrackCodecPrivate();

public:
  StreamAudioVorbis(Context&,
                    MediaSampleVorbis &id_header,
                    MediaSampleVorbis &comment_header,
                    MediaSampleVorbis &setup_header);

  int Receive(MediaSampleInterface* pSample);
  int EndOfStream();

private:
  typedef std::vector<unsigned char> header_t;
  header_t m_ident;
  header_t m_comment;
  header_t m_setup;

  class VorbisFrame : public AudioFrame
  {
    VorbisFrame& operator=(const VorbisFrame&);
    VorbisFrame(const VorbisFrame&);

  private:
    unsigned long m_timecode;
    unsigned char* m_data;
    unsigned long m_size;

  public:
    VorbisFrame(MediaSampleVorbis*, StreamAudioVorbis*);
    ~VorbisFrame();

    unsigned long GetTimecode() const;
    unsigned long GetSize() const;
    const unsigned char* GetData() const;
  };

  long long m_codec_private_data_pos;

  int FinalizeTrackCodecPrivate();
};

}  // namespace webmmux

#endif  // WEBMMUXSTREAMAUDIOVORBIS_HPP
