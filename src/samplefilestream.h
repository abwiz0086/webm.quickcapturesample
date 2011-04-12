/*
 *  Copyright (c) 2011 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 *  Taken directly from libwebmmux sample code.
 */

// SampleFileStream wraps a simple fstream in the EbmlStreamInterface found in
// webmmuxebmlio.hpp.

#ifndef QUICKCAPTURESAMPLE_SRC_SAMPLEFILESTREAM_H__
#define QUICKCAPTURESAMPLE_SRC_SAMPLEFILESTREAM_H__

#include <fstream>
#include <webmmuxebmlio.hpp>

class SampleFileStream : public webmmux::EbmlStreamInterface
{
public:
  explicit SampleFileStream(char* filename);
  ~SampleFileStream();

  bool IsOpen();

  int Seek(long long offset, webmmux::EbmlIOSeekPosition startPos,
           unsigned long long* newPos);
  int Write(void const* buf, unsigned long long bytesToWrite);
  int Read(void* buf, unsigned long bytesToRead);

private:
  std::fstream m_file_stream;
};
#endif  // QUICKCAPTURESAMPLE_SRC_SAMPLEFILESTREAM_H__
