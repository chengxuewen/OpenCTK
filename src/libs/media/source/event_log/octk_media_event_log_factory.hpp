/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_event_log.hpp>
#include <octk_media_context.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// This interface exists to allow webrtc to be optionally built without
// MediaEventLog support. A PeerConnectionFactory is constructed with an
// MediaEventLogFactoryInterface, which may or may not be null.
class MediaEventLogFactoryInterface
{
public:
    virtual ~MediaEventLogFactoryInterface() = default;

    virtual Nonnull<std::unique_ptr<MediaEventLog>> Create(const MediaContext &env) const = 0;
};


class OCTK_MEDIA_API MediaEventLogFactory : public MediaEventLogFactoryInterface
{
public:
    MediaEventLogFactory() = default;
    ~MediaEventLogFactory() override = default;

    Nonnull<std::unique_ptr<MediaEventLog>> Create(const MediaContext &env) const override;
};

OCTK_END_NAMESPACE
