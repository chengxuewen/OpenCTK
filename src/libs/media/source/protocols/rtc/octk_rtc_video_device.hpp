/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#pragma once

#include <octk_shared_pointer.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoCapturer
{
public:
    virtual ~RtcVideoCapturer() { }

    virtual bool StartCapture() = 0;

    virtual bool CaptureStarted() = 0;

    virtual void StopCapture() = 0;
};

class RtcVideoDevice
{
public:
    virtual uint32_t numberOfDevices() = 0;

    virtual int32_t getDeviceName(uint32_t deviceNumber,
                                  char *deviceNameUTF8,
                                  uint32_t deviceNameLength,
                                  char *deviceUniqueIdUTF8,
                                  uint32_t deviceUniqueIdUTF8Length,
                                  char *productUniqueIdUTF8 = 0,
                                  uint32_t productUniqueIdUTF8Length = 0) = 0;

    virtual SharedPointer<RtcVideoCapturer> create(const char *name,
                                                   uint32_t index,
                                                   size_t width,
                                                   size_t height,
                                                   size_t target_fps) = 0;

protected:
    virtual ~RtcVideoDevice() { }
};

OCTK_END_NAMESPACE
