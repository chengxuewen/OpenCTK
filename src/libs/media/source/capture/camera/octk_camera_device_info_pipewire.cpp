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

#include <private/octk_camera_device_info_pipewire_p.hpp>
#include <private/octk_xdg_desktop_portal_utils_p.hpp>
#include <private/octk_camera_capture_p.hpp>
#include "octk_pipewire_session_p.hpp"
#include <octk_logging.hpp>
#include <octk_checks.hpp>
#include <octk_memory.hpp>

#include <pipewire/core.h>
#include <pipewire/pipewire.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

namespace detail
{

} // namespace detail

class CameraDeviceInfoPipeWirePrivate : public CameraCapture::DeviceInfoPrivate
{
public:
    explicit CameraDeviceInfoPipeWirePrivate(CameraDeviceInfoPipeWire *p);
    ~CameraDeviceInfoPipeWirePrivate() override;

    SharedPointer<PipeWireSession> pipewire_session_;

private:
    OCTK_DECLARE_PUBLIC(CameraDeviceInfoPipeWire)
    OCTK_DISABLE_COPY_MOVE(CameraDeviceInfoPipeWirePrivate)
};

CameraDeviceInfoPipeWirePrivate::CameraDeviceInfoPipeWirePrivate(CameraDeviceInfoPipeWire *p)
    : CameraCapture::DeviceInfoPrivate(p)
    , pipewire_session_(utils::make_shared<PipeWireSession>())
{
}

CameraDeviceInfoPipeWirePrivate::~CameraDeviceInfoPipeWirePrivate()
{
}

CameraDeviceInfoPipeWire::CameraDeviceInfoPipeWire()
    : CameraCapture::DeviceInfo(new CameraDeviceInfoPipeWirePrivate(this))
{
    this->init();
}

CameraDeviceInfoPipeWire::~CameraDeviceInfoPipeWire()
{
}

uint32_t CameraDeviceInfoPipeWire::numberOfDevices()
{
    OCTK_D(CameraDeviceInfoPipeWire);
    OCTK_CHECK(d->pipewire_session_);
    return d->pipewire_session_->nodes().size();
}

int32_t CameraDeviceInfoPipeWire::getDeviceName(uint32_t deviceNumber,
                                                char *deviceNameUTF8,
                                                uint32_t deviceNameLength,
                                                char *deviceUniqueIdUTF8,
                                                uint32_t deviceUniqueIdUTF8Length,
                                                char *productUniqueIdUTF8,
                                                uint32_t productUniqueIdUTF8Length)
{
    OCTK_D(CameraDeviceInfoPipeWire);
    OCTK_CHECK(d->pipewire_session_);

    if (deviceNumber >= this->numberOfDevices())
    {
        return -1;
    }

    const auto &node = d->pipewire_session_->nodes().at(deviceNumber);

    if (deviceNameLength <= node->display_name().length())
    {
        OCTK_INFO() << "deviceNameUTF8 buffer passed is too small";
        return -1;
    }
    if (deviceUniqueIdUTF8Length <= node->unique_id().length())
    {
        OCTK_INFO() << "deviceUniqueIdUTF8 buffer passed is too small";
        return -1;
    }
    if (productUniqueIdUTF8 && productUniqueIdUTF8Length <= node->model_id().length())
    {
        OCTK_INFO() << "productUniqueIdUTF8 buffer passed is too small";
        return -1;
    }

    memset(deviceNameUTF8, 0, deviceNameLength);
    node->display_name().copy(deviceNameUTF8, deviceNameLength);

    memset(deviceUniqueIdUTF8, 0, deviceUniqueIdUTF8Length);
    node->unique_id().copy(deviceUniqueIdUTF8, deviceUniqueIdUTF8Length);

    if (productUniqueIdUTF8)
    {
        memset(productUniqueIdUTF8, 0, productUniqueIdUTF8Length);
        node->model_id().copy(productUniqueIdUTF8, productUniqueIdUTF8Length);
    }

    return 0;
}

int32_t CameraDeviceInfoPipeWire::init()
{
    OCTK_D(CameraDeviceInfoPipeWire);
    std::promise<PipeWireSession::Status> promise;
    d->pipewire_session_->Init([&promise](PipeWireSession::Status status) { promise.set_value(status); });
    const auto status = promise.get_future().get();
    OCTK_DEBUG("PipeWire::init() status:{}", fmt::as_int(status));
    return 0;
}

int32_t CameraDeviceInfoPipeWire::createCapabilityMap(const char *deviceUniqueIdUTF8)
{
    OCTK_D(CameraDeviceInfoPipeWire);
    OCTK_CHECK(d->pipewire_session_);

    for (auto &node : d->pipewire_session_->nodes())
    {
        if (node->unique_id().compare(deviceUniqueIdUTF8) != 0)
        {
            continue;
        }

        d->mCapabilities = node->capabilities();
        d->mLastUsedDeviceNameLength = node->unique_id().length();
        d->mLastUsedDeviceName = static_cast<char *>(realloc(d->mLastUsedDeviceName, d->mLastUsedDeviceNameLength + 1));
        memcpy(d->mLastUsedDeviceName, deviceUniqueIdUTF8, d->mLastUsedDeviceNameLength + 1);
        return d->mCapabilities.size();
    }
    return -1;
}

OCTK_END_NAMESPACE
