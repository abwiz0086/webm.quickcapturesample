// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMMUXMEDIASAMPLEINTERFACE_HPP
#define WEBMMUXMEDIASAMPLEINTERFACE_HPP

namespace webmmux
{
// MediaSampleInterface provides the most basic of attributes neede to create a
// SimpleBlock element: the media sample's absolute starting time and the media
// sample data.
class MediaSampleInterface
{
public:
  virtual ~MediaSampleInterface() {}

  // The absolute start time is expressed in nanoseconds, since a nanosecond
  // is the smallest interval of time that can be expressed in a webm file.
  // For reference, see http://matroska.org/technical/specs/index.html
  virtual long long GetAbsoluteStartTimeNs() const = 0;

  // Pass a pointer to the media sample data.
  virtual unsigned long GetBufferLength() const = 0;
  virtual unsigned char* GetBufferPtr() const = 0;
};

}  // namespace webmmux

#endif  // WEBMMUXMEDIASAMPLEINTERFACE_HPP
