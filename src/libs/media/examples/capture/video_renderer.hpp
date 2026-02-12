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

#ifndef _VIDEO_RENDERER_HPP
#define _VIDEO_RENDERER_HPP

#include <octk_video_sink_interface.hpp>
#include <octk_platform_thread.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_video_frame.hpp>
#include <octk_date_time.hpp>

#include <SDL3/SDL.h>

#include <atomic>
#include <mutex>

class VideoRenderer : public octk::VideoSinkInterface<octk::VideoFrame>
{
public:
    enum EventType
    {
        Event_Refresh = SDL_EVENT_USER + 1,
        Event_Quit = SDL_EVENT_USER + 2
    };

    enum class VideoType
    {
        I420,
        RGBA
    };

    VideoRenderer(VideoType videoType, const std::string &windowTitle, size_t width, size_t height)
        : mWindowTitle(windowTitle)
        , mVideoType(videoType)
    {
        mWindowWidth = width;
        mWindowHeight = height;
        this->resetVideoBuffer(width, height);
    }

    ~VideoRenderer() override
    {
        if (mVideoBuff)
        {
            free(mVideoBuff);
        }
        if (mSDLTexture)
        {
            SDL_DestroyTexture(mSDLTexture);
        }
        if (mSDLRender)
        {
            SDL_DestroyRenderer(mSDLRender);
        }
        if (mSDLWindow)
        {
            SDL_DestroyWindow(mSDLWindow);
        }
        SDL_Quit();
    }

    bool init()
    {
        bool success = false;
        std::call_once(mInitFlag,
                       [this, &success]()
                       {
                           if (!SDL_Init(SDL_INIT_VIDEO))
                           {
                               OCTK_ERROR("SDL_Init failed, err:%s", SDL_GetError());
                               return;
                           }

                           mSDLWindow = SDL_CreateWindow(mWindowTitle.c_str(),
                                                         mWindowWidth,
                                                         mWindowHeight,
                                                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
                           if (!mSDLWindow)
                           {
                               OCTK_ERROR("SDL_CreateWindow failed, err:%s", SDL_GetError());
                               return;
                           }

                           mSDLRender = SDL_CreateRenderer(mSDLWindow, "");
                           if (!mSDLRender)
                           {
                               OCTK_ERROR("SDL_CreateRenderer failed, err:%s", SDL_GetError());
                               return;
                           }

                           if (VideoType::I420 == mVideoType)
                           {
                               mSDLTexture = SDL_CreateTexture(mSDLRender,
                                                               SDL_PIXELFORMAT_IYUV,
                                                               SDL_TEXTUREACCESS_STREAMING,
                                                               mVideoWidth,
                                                               mVideoHeight);
                           }
                           else if (VideoType::RGBA == mVideoType)
                           {
                               mSDLTexture = SDL_CreateTexture(mSDLRender,
                                                               SDL_PIXELFORMAT_RGBA32,
                                                               SDL_TEXTUREACCESS_STREAMING,
                                                               mVideoWidth,
                                                               mVideoHeight);
                           }
                           else
                           {
                               assert(false);
                           }
                           if (!mSDLTexture)
                           {
                               OCTK_ERROR("SDL_CreateTexture failed, err:%s", SDL_GetError());
                               return;
                           }

                           success = true;
                       });
        return success;
    }

    void loop()
    {
        mLooping.store(true);
        while (mLooping.load(std::memory_order_relaxed))
        {
            SDL_WaitEvent(&mSDLEvent);
            switch (mSDLEvent.type)
            {
                case SDL_EVENT_QUIT:
                {
                    mExit.store(true);
                    break;
                }
                case SDL_EVENT_KEY_DOWN:
                {
                    if (SDLK_Q == mSDLEvent.key.key)
                    {
                        OCTK_DEBUG("key down q and push quit event");
                        SDL_Event quit_event;
                        quit_event.type = SDL_EVENT_QUIT;
                        SDL_PushEvent(&quit_event);
                    }
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED:
                {
                    SDL_GetWindowSize(mSDLWindow, &mWindowWidth, &mWindowHeight);
                    OCTK_DEBUG("SDL_EVENT_WINDOW_RESIZED width:%d, height:%d", mWindowWidth, mWindowHeight);
                    break;
                }
                case Event_Quit:
                {
                    printf("M_QUIT_EVENT---\n");
                    mLooping = false;
                    break;
                }
                case Event_Refresh:
                {
                    if (!mVideoBuff)
                    {
                        break;
                    }
                    if (0)
                    {
                        OCTK_TRACE("Event_Refresh width:%d, height:%d", mWindowWidth, mWindowHeight);
                    }
                    if (VideoType::I420 == mVideoType)
                    {
                        SDL_UpdateTexture(mSDLTexture, nullptr, mVideoBuff, mVideoWidth);
                    }
                    else if (VideoType::RGBA == mVideoType)
                    {
                        SDL_UpdateTexture(mSDLTexture, nullptr, mVideoBuff, mVideoWidth * 4);
                    }
                    else
                    {
                        assert(false);
                    }

                    mSDLRect.x = 0;
                    mSDLRect.y = 0;
                    float w_ratio = mWindowWidth * 1.0 / mVideoWidth;
                    float h_ratio = mWindowHeight * 1.0 / mVideoHeight;
                    mSDLRect.w = mVideoWidth * w_ratio;
                    mSDLRect.h = mVideoHeight * h_ratio;

                    SDL_RenderClear(mSDLRender);
                    SDL_RenderTexture(mSDLRender, mSDLTexture, nullptr, &mSDLRect);
                    SDL_RenderPresent(mSDLRender);
                    break;
                }
                default: break;
            }
        }
    }

    void onFrame(const octk::VideoFrame &frame) override
    {
        auto frameBuffer = frame.videoFrameBuffer();
        if (VideoType::I420 == mVideoType)
        {
            auto i420Buffer = frameBuffer->toI420();
            this->resetVideoBuffer(frame.width(), frame.height(), i420Buffer->dataY());
        }
        else if (VideoType::RGBA == mVideoType)
        {
            auto rgbaBuffer = frameBuffer->toRGBA();
            this->resetVideoBuffer(frame.width(), frame.height(), rgbaBuffer->data());
        }
        else
        {
            assert(false);
        }
        const auto timestampUSecs = frame.timestampUSecs();
        const auto timestampMSecs = timestampUSecs / octk::DateTime::kUSecsPerMSec;
        if (0)
        {
            OCTK_TRACE("VideoRenderer::onFrame:type=%d, width:%d, height:%d, ntp:%lld, ts:%lld(%s)",
                       octk::utils::fmt::as_int(frameBuffer->type()),
                       frame.width(),
                       frame.height(),
                       frame.ntpTimeMSecs(),
                       timestampUSecs,
                       octk::DateTime::localTimeStringFromSteadyTimeMSecs(timestampMSecs).c_str());
        }

        SDL_Event event;
        event.type = Event_Refresh;
        SDL_PushEvent(&event);
    }

    // Should be called by the source when it discards the frame due to rate limiting.
    void onDiscardedFrame() override { }

    // Called on the network thread when video constraints change.
    void onConstraintsChanged(const octk::VideoTrackSourceConstraints & /* constraints */) override { }

protected:
    void resetVideoBuffer(int width, int height, const uint8_t *buffer = nullptr)
    {
        if (width != mVideoWidth || height != mVideoHeight)
        {
            mVideoWidth = width;
            mVideoHeight = height;
            if (VideoType::I420 == mVideoType)
            {
                const auto yLength = width * height;
                const auto uLength = width * height / 4;
                const auto vLength = width * height / 4;
                mFrameLength = yLength + uLength + vLength;
            }
            else if (VideoType::RGBA == mVideoType)
            {
                mFrameLength = width * height * 4;
            }
            else
            {
                assert(false);
            }
            mVideoBuff = (uint8_t *)std::realloc(mVideoBuff, mFrameLength);
            if (!mVideoBuff)
            {
                OCTK_FATAL("malloc video_buff failed");
            }
        }
        if (buffer)
        {
            memcpy(mVideoBuff, buffer, mFrameLength);
        }
    }

    int mWindowWidth;
    int mWindowHeight;
    int mVideoWidth = -1;
    int mVideoHeight = -1;
    int mFrameLength;
    std::once_flag mInitFlag;
    std::atomic<bool> mLooping;
    std::atomic<bool> mExit;
    uint8_t *mVideoBuff{nullptr};
    const std::string mWindowTitle;
    const VideoType mVideoType{VideoType::I420};

    SDL_FRect mSDLRect;
    SDL_Event mSDLEvent;
    SDL_Window *mSDLWindow = NULL;
    SDL_Renderer *mSDLRender = NULL;
    SDL_Texture *mSDLTexture = NULL;
    SDL_Thread *mSDLTimerThread = NULL;
};

#undef main

#endif // _VIDEO_RENDERER_HPP
