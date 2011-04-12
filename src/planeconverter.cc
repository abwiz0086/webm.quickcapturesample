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

#include "logger.h"
#include "planeconverter.h"

int PlaneConverter::YvyuToYv12(unsigned char *vy12_output, char *yvyu_input,
                               int width, int height) {
  return ConvertToYv12(vy12_output, yvyu_input, width, height, YVYU);
}

int PlaneConverter::YuyToYv12(unsigned char *vy12_output, char *yuyv_input,
                              int width, int height) {
  return ConvertToYv12(vy12_output, yuyv_input, width, height, YUY);
}

int PlaneConverter::ConvertToYv12(unsigned char *vy12_output,
    char *yuyv_input, int width, int height, FORMAT format) {

  LOG_ASSERT_INFO(format == YUY || format == YVYU, "Format not supported");

  const int u_position = format == YUY ? 3 : 1;
  const int v_position = format == YUY ? 1 : 3;
  static const int kYFirstPosition = 0;
  static const int kYSecondPosition = 2;

  unsigned char *y = vy12_output;
  unsigned char *u = width * height + y;
  unsigned char *v = width / 2 * height / 2 + u;
  int i, j;

  char *yuyv_copy = yuyv_input;

  // TODO: Improve conversion algorithm?
  for (; y < u; yuyv_copy += 4) {
    *y++ = yuyv_copy[kYFirstPosition];
    *y++ = yuyv_copy[kYSecondPosition];
  }

  yuyv_copy = yuyv_input;

  for (i = 0; i<(height >> 1); i++, yuyv_copy += (width << 1)) {
    for (j = 0; j<(width >> 1); j++, yuyv_copy += 4) {
      *u++ = yuyv_copy[u_position];
    }
  }

  yuyv_copy = yuyv_input;

  for (i = 0; i<(height >> 1); i++, yuyv_copy += (width << 1)) {
    for (j = 0; j<(width >> 1); j++, yuyv_copy += 4) {
      *v++ = yuyv_copy[v_position];
    }
  }
  return 0;
}
