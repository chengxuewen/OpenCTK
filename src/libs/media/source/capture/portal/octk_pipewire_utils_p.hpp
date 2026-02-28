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

#include <octk_media_global.hpp>

#include <errno.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// static
struct dma_buf_sync
{
    uint64_t flags;
};
#define DMA_BUF_SYNC_READ  (1 << 0)
#define DMA_BUF_SYNC_START (0 << 2)
#define DMA_BUF_SYNC_END   (1 << 2)
#define DMA_BUF_BASE       'b'
#define DMA_BUF_IOCTL_SYNC _IOW(DMA_BUF_BASE, 0, struct dma_buf_sync)

struct pw_thread_loop;

OCTK_BEGIN_NAMESPACE

constexpr int kInvalidPipeWireFd = -1;

// Prepare PipeWire so that it is ready to be used. If it needs to be dlopen'd
// this will do so. Note that this does not guarantee a PipeWire server is
// running nor does it establish a connection to one.
bool InitializePipeWire();

// Locks pw_thread_loop in the current scope
class PipeWireThreadLoopLock
{
public:
    explicit PipeWireThreadLoopLock(pw_thread_loop *loop);
    ~PipeWireThreadLoopLock();

private:
    pw_thread_loop *const loop_;
};

// We should synchronize DMA Buffer object access from CPU to avoid potential
// cache incoherency and data loss.
// See
// https://01.org/linuxgraphics/gfx-docs/drm/driver-api/dma-buf.html#cpu-access-to-dma-buffer-objects
static bool SyncDmaBuf(int fd, uint64_t start_or_end)
{
    struct dma_buf_sync sync = {0};

    sync.flags = start_or_end | DMA_BUF_SYNC_READ;

    while (true)
    {
        int ret;
        ret = ioctl(fd, DMA_BUF_IOCTL_SYNC, &sync);
        if (ret == -1 && errno == EINTR)
        {
            continue;
        }
        else if (ret == -1)
        {
            return false;
        }
        else
        {
            break;
        }
    }

    return true;
}

class ScopedBuf
{
public:
    ScopedBuf() { }
    ScopedBuf(uint8_t *map, int map_size, int fd, bool is_dma_buf = false)
        : map_(map)
        , map_size_(map_size)
        , fd_(fd)
        , is_dma_buf_(is_dma_buf)
    {
    }
    ~ScopedBuf()
    {
        if (map_ != MAP_FAILED)
        {
            if (is_dma_buf_)
            {
                SyncDmaBuf(fd_, DMA_BUF_SYNC_END);
            }
            munmap(map_, map_size_);
        }
    }

    explicit operator bool() { return map_ != MAP_FAILED; }

    void initialize(uint8_t *map, int map_size, int fd, bool is_dma_buf = false)
    {
        map_ = map;
        map_size_ = map_size;
        is_dma_buf_ = is_dma_buf;
        fd_ = fd;

        if (is_dma_buf_)
        {
            SyncDmaBuf(fd_, DMA_BUF_SYNC_START);
        }
    }

    uint8_t *get() { return map_; }

protected:
    uint8_t *map_ = static_cast<uint8_t *>(MAP_FAILED);
    int map_size_;
    int fd_;
    bool is_dma_buf_;
};

OCTK_END_NAMESPACE