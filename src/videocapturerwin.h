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

// Captures video using dshow on a windows machine, and stores it in
// raw_->img_data in YVY12 format.

#ifndef QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERWIN__
#define QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERWIN__

#include <atlbase.h> // For CComVariant and CComQIPtr
// To avoid 'Need to include tchar.h after strsafe.h' error, this must come
// before dshow.h.
#include <tchar.h>
#include <dshow.h>

#include "config.h"
#include "videocapturer.h"
#include "qedit.h"

class VideoCapturerWin : public VideoCapturer, public ISampleGrabberCB {
 public:
  VideoCapturerWin(Config config);
  ~VideoCapturerWin();

  virtual void Init();
  virtual void StartCapture();
  virtual bool BufferHasFrame() { return buffer_has_frame_; }

 private:
  void CreateFilters(IBaseFilter** grabber, IBaseFilter** filter);
  void FormatMediaStream(IBaseFilter* grabber, IBaseFilter* filter);
  void InitSampleGrabber(IBaseFilter* grabber);
  void FillVideoInfoHeader(VIDEOINFOHEADER* video_info_header);
  void FillMediaType(AM_MEDIA_TYPE* media_type);
  void ExitOnFailure(HRESULT result);

  HRESULT FindFilter(CLSID cls, IBaseFilter **pp, bool name,
                     CComVariant &filter);
  HRESULT GetFirstPin(IBaseFilter *capture, PIN_DIRECTION pin_direction,
                      IPin **capture_out_pin);

  // Overridden methods for ISampleGrabberCB
  STDMETHODIMP_(ULONG) AddRef() { return 2; }
  STDMETHODIMP_(ULONG) Release() { return 1; }
  STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
  STDMETHODIMP SampleCB(double sample_time, IMediaSample *sample) {
    return S_OK;
  }
  STDMETHODIMP BufferCB(double sample_time, BYTE *buffer, long buffer_len);

  // TODO: Use com_ptr_t rather than CComQIPtr and CComPtr to avoid any
  // potential issues with memory leakage that often accompanies CCom* usage.
  auto_ptr<CComQIPtr<ISampleGrabberCB, &IID_ISampleGrabberCB>>
      video_callback_ptr_;
  CComPtr<IBaseFilter> capture_;
  CComPtr<IGraphBuilder> graph_;

  CComPtr<IAMStreamConfig> iam_stream_config_;
};

#endif // QUICKCAPTURESAMPLE_SRC_VIDEOCAPTURERWIN__
