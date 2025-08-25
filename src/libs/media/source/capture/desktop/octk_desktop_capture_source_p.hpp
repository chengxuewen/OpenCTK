/***********************************************************************************************************************
** Copyright (C) 2025~Present MaxSense, Created by ChengXueWen on 25-4-8.
***********************************************************************************************************************/

#ifndef _MSRTC_DESKTOP_CAPTURE_SOURCE_P_HPP
#define _MSRTC_DESKTOP_CAPTURE_SOURCE_P_HPP

#include <msrtc_desktop_capture_source.hpp>
#include <msrtc_engine_config.hpp>

#include <octk_i420_buffer.hpp>
#include <octk_video_adapter.hpp>
#include <octk_video_broadcaster.hpp>

#include <atomic>
#include <thread>

#if MSRTC_FEATURE_USE_LIBWEBRTC
#   include <api/video/i420_buffer.h>
#   include <modules/desktop_capture/desktop_capture_options.h>
#   include <modules/desktop_capture/desktop_capturer.h>
#   include <modules/desktop_capture/desktop_frame.h>
#endif

MSRTC_BEGIN_NAMESPACE

using octk::I420Buffer;
using octk::VideoAdapter;
using octk::VideoBroadcaster;

class DesktopCaptureSourcePrivate
#if MSRTC_FEATURE_USE_LIBWEBRTC
    : public webrtc::DesktopCapturer::Callback
#endif
{
public:
    explicit DesktopCaptureSourcePrivate(DesktopCaptureSource *p);
    ~DesktopCaptureSourcePrivate();

    void updateVideoAdapter();
    void updateLastError(const std::string &error);

    void implStart();
    void implInit(size_t targetFps, size_t deviceIndex);

#if MSRTC_FEATURE_USE_LIBWEBRTC
    // Called before a frame capture is started.
    void OnFrameCaptureStart() override;
    // Called after a frame has been captured. `frame` is not nullptr if and only if `result` is SUCCESS.
    virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                                 std::unique_ptr<webrtc::DesktopFrame> frame) override;

    std::shared_ptr<webrtc::I420Buffer> mLibWebRTCI420Buffer;
    std::unique_ptr<webrtc::DesktopCapturer> mLibWebRTCDesktopCapturer;
#endif

    size_t mFps = 0;
    size_t mIndex = 0;
    int64_t mIntervalMSecs;
    std::string mLastError;
    std::string mWindowTitle;
    std::once_flag mInitOnceFlag;
    std::atomic<bool> mStartFlag;
    std::shared_ptr<I420Buffer> mI420Buffer;
    std::unique_ptr<std::thread> mCaptureThread;
    std::atomic<bool> mIsInited{false};
    std::atomic<int64_t> mFPSTimestamp{0};
    std::atomic<int64_t> mCaptureElapsedMSecs{0};
    std::atomic<int64_t> mCaptureConvertElapsedMSecs{0};

    VideoAdapter mVideoAdapter;
    VideoBroadcaster mVideoBroadcaster;

protected:
    MSRTC_DEFINE_PPTR(DesktopCaptureSource)
    MSRTC_DECLARE_PUBLIC(DesktopCaptureSource)

    MSRTC_DISABLE_COPY_MOVE(DesktopCaptureSourcePrivate)
};
MSRTC_END_NAMESPACE

#endif // _MSRTC_DESKTOP_CAPTURE_SOURCE_P_HPP
