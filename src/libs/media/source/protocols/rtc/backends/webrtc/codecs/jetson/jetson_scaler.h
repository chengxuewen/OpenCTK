#pragma once

#include "v4l2_frame_buffer.h"
#include "worker.h"

#include <NvBufSurface.h>

#include <functional>
#include <memory>

OCTK_USE_NAMESPACE

class JetsonScaler : public IFrameProcessor
{
public:
    struct CaptureTask
    {
        int dst_dma_fd;
        std::function<void()> callback;
    };

    static std::unique_ptr<JetsonScaler> Create(int src_width, int src_height, int dst_width, int dst_height);

    JetsonScaler();
    ~JetsonScaler() override;

    void EmplaceBuffer(V4L2FrameBufferRef buffer, std::function<void(V4L2FrameBufferRef)> on_capture) override;

protected:
    void CaptureBuffer();
    void Start();

private:
    int dst_width_;
    int dst_height_;
    int num_buffer_;
    std::atomic<bool> abort_;
    std::unique_ptr<Worker> worker_;
    NvBufSurf::NvCommonTransformParams transform_params_;
    LockFreeQueue<int> free_buffers_;
    LockFreeQueue<CaptureTask> capturing_tasks_;

    bool Configure(int src_width, int src_height, int dst_width, int dst_height);
};
