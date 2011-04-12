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

// Contains functions for static conversions between different formats.

#ifndef QUICKCAPTURESAMPLE_SRC_PLANECONVERTER_H__
#define QUICKCAPTURESAMPLE_SRC_PLANECONVERTER_H__

class PlaneConverter {
public:
  enum FORMAT {
    YVYU,
    YUY
  };

  static int YvyuToYv12(unsigned char *vy12_output, char *yuyv_input,
                        int width, int height);
  static int YuyToYv12(unsigned char *vy12_output, char *yuyv_input,
                       int width, int height);
 private:
  static int ConvertToYv12(unsigned char *vy12_output, char *yuyv_input,
                           int width, int height, FORMAT format);
};

#endif // QUICKCAPTURESAMPLE_SRC_PLANECONVERTER_H__
