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

#ifndef _OCTK_CUSTOM_VIDEO_CAPTURER_HPP
#define _OCTK_CUSTOM_VIDEO_CAPTURER_HPP

#include <octk_video_source_interface.hpp>
#include <octk_video_broadcaster.hpp>
#include <octk_video_adapter.hpp>
#include <octk_video_frame.hpp>
#include <octk_mutex.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API CustomVideoCapturer : public VideoSourceInterface<VideoFrame>
{
public:
    class FramePreprocessor
    {
    public:
        virtual ~FramePreprocessor() = default;

        virtual VideoFrame Preprocess(const VideoFrame &frame) = 0;
    };

    ~CustomVideoCapturer() override;

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;
    void setFramePreprocessor(std::unique_ptr<FramePreprocessor> preprocessor)
    {
        Mutex::Lock locker(mMutex);
        mPreprocessor = std::move(preprocessor);
    }
    void setEnableAdaptation(bool enable_adaptation)
    {
        Mutex::Lock locker(mMutex);
        mEnableAdaptation = enable_adaptation;
    }
    void onOutputFormatRequest(int width, int height, const Optional<int> &max_fps);

    // Starts or resumes video capturing. Can be called multiple times during
    // lifetime of this object.
    virtual void start() = 0;
    // Stops or pauses video capturing. Can be called multiple times during
    // lifetime of this object.
    virtual void stop() = 0;

    virtual int getFrameWidth() const = 0;
    virtual int getFrameHeight() const = 0;

protected:
    void onFrame(const VideoFrame &frame);
    VideoSinkWants getSinkWants();

private:
    void updateVideoAdapter();
    VideoFrame maybePreprocess(const VideoFrame &frame);

    Mutex mMutex;
    std::unique_ptr<FramePreprocessor> mPreprocessor OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    bool mEnableAdaptation OCTK_ATTRIBUTE_GUARDED_BY(mMutex) = true;
    VideoBroadcaster mBroadcaster;
    VideoAdapter mVideoAdapter;
};

OCTK_END_NAMESPACE

#endif // _OCTK_CUSTOM_VIDEO_CAPTURER_HPP
