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
#include "videocapturerwin.h"

VideoCapturerWin::VideoCapturerWin(Config config) : VideoCapturer(config) {
  video_callback_ptr_.reset(
      new CComQIPtr<ISampleGrabberCB, &IID_ISampleGrabberCB>(this));
};


VideoCapturerWin::~VideoCapturerWin() {
  CoUninitialize();
}

void VideoCapturerWin::Init() {
  if (FAILED(CoInitialize(NULL))) {
    LOG("CoInitialize failed");
    exit(EXIT_FAILURE);
  }
}

void VideoCapturerWin::StartCapture() {
  CComPtr<IBaseFilter> grabber, video_filter;
  LOG("Creating filters...");
  CreateFilters(&grabber, &video_filter);
  FormatMediaStream(grabber, video_filter);
  InitSampleGrabber(grabber);

  LOG("Running graph...");
  ExitOnFailure(
      static_cast<CComQIPtr<IMediaControl, &IID_IMediaControl>>(graph_)->Run());
}

HRESULT VideoCapturerWin::FindFilter(CLSID cls, IBaseFilter **pp, bool name,
                                     CComVariant &filter) {
  HRESULT hr;
  CComPtr<ICreateDevEnum> dev_enum;
  hr = dev_enum.CoCreateInstance(CLSID_SystemDeviceEnum);

  if (!SUCCEEDED(hr))
    return hr;

  CComPtr<IEnumMoniker> enum_moniker;
  hr = dev_enum->CreateClassEnumerator(cls, &enum_moniker, 0);

  if (!SUCCEEDED(hr))
    return hr;

  CComPtr<IMoniker> moniker;
  ULONG fetched;

  while (enum_moniker->Next(1, &moniker, &fetched) == S_OK) {
    CComPtr<IPropertyBag> iPropBag;
    hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&iPropBag);
    if (SUCCEEDED(hr)) {
      // To retrieve the filter's friendly name, do the following:
      CComVariant var_name;
      hr = iPropBag->Read(L"FriendlyName", &var_name, 0);

      if (SUCCEEDED(hr)) {
        if (!name || var_name == filter) {
          // To create an instance of the filter, do the following:
          hr = moniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **)pp);
          break;
        }
      }
    }

    moniker = NULL;
  }

  return hr;
}

HRESULT VideoCapturerWin::GetFirstPin(IBaseFilter *capture,
                                      PIN_DIRECTION pin_direction,
                                      IPin **capture_out_pin) {
  CComPtr<IEnumPins> enum_moniker;
  capture->EnumPins(&enum_moniker);
  CComPtr<IPin> pin;

  while (enum_moniker && enum_moniker->Next(1, &pin, NULL) == S_OK) {
    PIN_INFO pin_info;
    pin->QueryPinInfo(&pin_info);

    if (pin_info.dir == pin_direction) {
      *capture_out_pin = pin.Detach();
      return S_OK;
    }
    pin = NULL;
  }
  return E_FAIL;
}

void VideoCapturerWin::CreateFilters(IBaseFilter** grabber,
                                     IBaseFilter** filter) {
  LOG("Creating filters...");
  ExitOnFailure(FindFilter(CLSID_VideoInputDeviceCategory, &capture_, false,
                CComVariant(L"")));
  ExitOnFailure(CoCreateInstance(CLSID_SampleGrabber, 0, CLSCTX_INPROC_SERVER,
      IID_IBaseFilter, reinterpret_cast<void **>(grabber)));
  ExitOnFailure(CoCreateInstance(CLSID_VideoRenderer, 0, CLSCTX_INPROC_SERVER,
                                 IID_IBaseFilter,
                                 reinterpret_cast<void **>(filter)));

  ExitOnFailure(graph_.CoCreateInstance(CLSID_FilterGraph));
  ExitOnFailure(graph_->AddFilter(capture_, L"Capture"));
  ExitOnFailure(graph_->AddFilter(*grabber, L"SampleGrabber"));
  ExitOnFailure(graph_->AddFilter(*filter, L"Video Renderer"));
}

void VideoCapturerWin::FormatMediaStream(IBaseFilter* grabber,
                                         IBaseFilter* filter) {
  LOG("Connecting pins...");
  CComPtr<IPin> cap_out_pin, grab_in_pin, grab_out_pin, null_in_pin,
                null_out_pin;
  ExitOnFailure(GetFirstPin(capture_, PINDIR_OUTPUT, &cap_out_pin));
  ExitOnFailure(grabber->FindPin(L"In", &grab_in_pin));
  ExitOnFailure(grabber->FindPin(L"Out", &grab_out_pin));
  ExitOnFailure(filter->FindPin(L"In", &null_in_pin));

  ExitOnFailure(cap_out_pin->QueryInterface(IID_IAMStreamConfig,
                                            (void **)&iam_stream_config_));

  AM_MEDIA_TYPE media_type;
  FillMediaType(&media_type);
  ExitOnFailure(iam_stream_config_->SetFormat(&media_type));

  ExitOnFailure(graph_->Connect(cap_out_pin, grab_in_pin));
  ExitOnFailure(graph_->Connect(grab_out_pin, null_in_pin));

  CComPtr<IVideoWindow> video_window;
  ExitOnFailure(filter->QueryInterface(IID_IVideoWindow,
                                       (void **)&video_window));
}

void VideoCapturerWin::InitSampleGrabber(IBaseFilter* grabber) {
  CComPtr<ISampleGrabber> sample_grabber;
  ExitOnFailure(grabber->QueryInterface(IID_ISampleGrabber,
                                        (void **)&sample_grabber));
  sample_grabber->SetBufferSamples(true);
  ExitOnFailure(sample_grabber->SetCallback(this, 1));
}

void VideoCapturerWin::FillMediaType(AM_MEDIA_TYPE* media_type) {
  ZeroMemory(media_type, sizeof(AM_MEDIA_TYPE));
  media_type->subtype = MEDIASUBTYPE_YUY2;
  media_type->majortype = MEDIATYPE_Video;
  media_type->formattype = FORMAT_VideoInfo;
  media_type->pbFormat =
      reinterpret_cast<BYTE *>(CoTaskMemAlloc(sizeof(VIDEOINFOHEADER)));
  ZeroMemory(media_type->pbFormat, sizeof(VIDEOINFOHEADER));
  media_type->cbFormat = sizeof(VIDEOINFOHEADER);
  media_type->bFixedSizeSamples = 1;
  media_type->bTemporalCompression = 0;

  VIDEOINFOHEADER *video_info_header =
      reinterpret_cast<VIDEOINFOHEADER *>(media_type->pbFormat);
  FillVideoInfoHeader(video_info_header);
  media_type->lSampleSize = video_info_header->bmiHeader.biSizeImage;
}

void VideoCapturerWin::FillVideoInfoHeader(VIDEOINFOHEADER* video_info_header) {
  video_info_header->bmiHeader.biCompression = mmioFOURCC('Y', 'U', 'Y', '2');
  video_info_header->bmiHeader.biWidth = config_.display_width();
  video_info_header->bmiHeader.biHeight = config_.display_height();
  video_info_header->bmiHeader.biBitCount = 12;
  video_info_header->bmiHeader.biPlanes = 3;
  video_info_header->bmiHeader.biSizeImage =
      video_info_header->bmiHeader.biWidth *
      video_info_header->bmiHeader.biHeight *
      video_info_header->bmiHeader.biBitCount / 8;
  video_info_header->AvgTimePerFrame = 10000000 / config_.capture_frame_rate();
  video_info_header->rcSource.top = 0;
  video_info_header->rcSource.left = 0;
  video_info_header->rcSource.bottom = config_.display_height();
  video_info_header->rcSource.right = config_.display_width();
  video_info_header->rcTarget.top = 0;
  video_info_header->rcTarget.left = 0;
  video_info_header->rcTarget.bottom = config_.display_height();
  video_info_header->rcTarget.right = config_.display_width();
}

STDMETHODIMP VideoCapturerWin::QueryInterface(REFIID riid, void **ppv) {
  if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown) {
    *ppv = (void *)static_cast<ISampleGrabberCB *>(this);
    return NOERROR;
  }
  return E_NOINTERFACE;
}

STDMETHODIMP VideoCapturerWin::BufferCB(double sample_time,
    BYTE *buffer, long buffer_len) {
  if (UserEnteredKey()) {
    finished_ = true;
    return S_OK;
  }
  buffer_has_frame_ = false;
  PlaneConverter::YuyToYv12(raw_->img_data, (char*)buffer,
                            raw_->d_w, raw_->d_h);
  buffer_seconds_ = sample_time;
  buffer_has_frame_ = true;
  return S_OK;
}

void VideoCapturerWin::ExitOnFailure(HRESULT result) {
  if (FAILED(result)) {
    LOG("Bad hresult: %d", result);
    exit(EXIT_FAILURE);
  };
}
