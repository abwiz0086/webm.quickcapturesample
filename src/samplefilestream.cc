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

#include <cstdlib>
#include <limits>
#include "samplefilestream.h"

SampleFileStream::SampleFileStream(char* filename)
    : m_file_stream(filename, std::ios_base::in | std::ios_base::out |
                    std::ios_base::binary | std::ios_base::trunc)
{
}

SampleFileStream::~SampleFileStream()
{
  m_file_stream.close();
}

bool SampleFileStream::IsOpen()
{
  if (!m_file_stream)
    return(false);
  return(true);
}

int SampleFileStream::Seek(long long offset,
                           webmmux::EbmlIOSeekPosition startPos,
                           unsigned long long* newPos)
{
  m_file_stream.seekp(static_cast<std::streamoff>(offset),
                      static_cast<std::ios_base::seekdir>(startPos));

  if (m_file_stream.fail())
  {
    m_file_stream.clear();
    long long size = startPos + offset;
    if (size >
        static_cast<long long>(std::numeric_limits<std::streamsize>::max()))
    {
      printf("ERROR: Seek request is larger than max std::streamsize.\n");
      exit(1);
    }
    m_file_stream.seekp(0, std::ios_base::end);
    if (size > static_cast<unsigned int>(m_file_stream.tellp()))
    {
      unsigned long long extraSize = size - m_file_stream.tellp();
      long currentWidth = m_file_stream.width();
      m_file_stream.width(static_cast<std::streamsize>(extraSize));
      m_file_stream << ' ';
      m_file_stream.width(currentWidth);
    }
    m_file_stream.seekp(std::ios_base::end);
  }
  if (newPos)
    *newPos = m_file_stream.tellp();
  return(0);
}

int SampleFileStream::Write(void const* buf, unsigned long long bytesToWrite)
{
  if (bytesToWrite > static_cast<unsigned long long>(
                         std::numeric_limits<std::streamsize>::max()))
  {
    printf("ERROR: Write request is larger than max std::streamsize.\n");
    exit(1);
  }
  char *charbuf = static_cast<char*>(const_cast<void*>(buf));
  m_file_stream.write(charbuf, static_cast<std::streamsize>(bytesToWrite));
  return(m_file_stream.fail());
}

int SampleFileStream::Read(void* buf, unsigned long bytesToRead)
{
  // set read pointer to current put pointer position
  m_file_stream.seekg(m_file_stream.tellp());

  m_file_stream.read(static_cast<char*>(buf), bytesToRead);

  // after read, set put pointer to last read position
  m_file_stream.seekp(m_file_stream.tellg());

  return(!m_file_stream.good());
}
