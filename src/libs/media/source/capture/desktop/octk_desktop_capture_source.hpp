/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_DESKTOP_CAPTURE_SOURCE_HPP
#define _OCTK_DESKTOP_CAPTURE_SOURCE_HPP

#include <octk_video_source_interface.hpp>
#include <octk_video_sink_interface.hpp>
#include <octk_video_frame.hpp>
#include <octk_logging.hpp>

OCTK_BEGIN_NAMESPACE

class DesktopCaptureSourcePrivate;

class OCTK_MEDIA_API DesktopCaptureSource :
    public VideoSourceInterface<VideoFrame>, public VideoSinkInterface<VideoFrame>
{
public:
    using SharedPtr = std::shared_ptr<DesktopCaptureSource>;

    DesktopCaptureSource();
    DesktopCaptureSource(size_t
                         targetFps,
                         size_t deviceIndex);
    ~DesktopCaptureSource() override;

    std::string windowTitle() const;
    std::string lastError() const;
    bool isInited() const;
    size_t index() const;
    size_t fps() const;

    bool init(size_t
              targetFps,
              size_t deviceIndex);
    bool startCapture();
    void stopCapture();

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;
    void requestRefreshFrame() override;

    void onConstraintsChanged(const VideoTrackSourceConstraints & /* constraints */) override;
    void onFrame(const VideoFrame &frame) override;
    void onDiscardedFrame() override;

protected:
    OCTK_DEFINE_DPTR(DesktopCaptureSource)
    OCTK_DECLARE_PRIVATE(DesktopCaptureSource)

    OCTK_DISABLE_COPY_MOVE(DesktopCaptureSource)
};
OCTK_END_NAMESPACE

#endif // _OCTK_DESKTOP_CAPTURE_SOURCE_HPP
