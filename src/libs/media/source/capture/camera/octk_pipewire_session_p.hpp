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

#include <private/octk_camera_portal_p.hpp>
#include <private/octk_pipewire_utils_p.hpp>
#include <octk_camera_capture.hpp>

#include <pipewire/core.h>
#include <pipewire/pipewire.h>

#include <glib.h>
#include <gio/gio.h>

#include <deque>
#include <atomic>
#include <thread>
#include <mutex>

OCTK_BEGIN_NAMESPACE

class PipeWireSession;
class VideoCaptureModulePipeWire;

// PipeWireNode objects are the local representation of PipeWire node objects.
// The portal API ensured that only camera nodes are visible to the client.
// So they all represent one camera that is available via PipeWire.
class PipeWireNode
{
public:
    struct PipeWireNodeDeleter
    {
        void operator()(PipeWireNode *node) const noexcept;
    };

    using PipeWireNodePtr = std::unique_ptr<PipeWireNode, PipeWireNode::PipeWireNodeDeleter>;
    static PipeWireNodePtr Create(PipeWireSession *session, uint32_t id, const spa_dict *props);

    std::vector<CameraCapture::Capability> capabilities() const { return capabilities_; }
    std::string display_name() const { return display_name_; }
    std::string unique_id() const { return unique_id_; }
    std::string model_id() const { return model_id_; }
    uint32_t id() const { return id_; }

protected:
    PipeWireNode(PipeWireSession *session, uint32_t id, const spa_dict *props);

private:
    static void OnNodeInfo(void *data, const pw_node_info *info);
    static void OnNodeParam(void *data, int seq, uint32_t id, uint32_t index, uint32_t next, const spa_pod *param);
    static bool ParseFormat(const spa_pod *param, CameraCapture::Capability *cap);

    pw_proxy *proxy_;
    spa_hook node_listener_;
    PipeWireSession *session_;
    uint32_t id_;
    std::string display_name_;
    std::string unique_id_;
    std::string model_id_;
    std::vector<CameraCapture::Capability> capabilities_;
};

class CameraPortalNotifier : public portal::CameraPortal::PortalNotifier
{
public:
    CameraPortalNotifier(PipeWireSession *session);
    ~CameraPortalNotifier() = default;

    void OnCameraRequestResult(portal::xdg::RequestResponse result, int fd) override;

private:
    PipeWireSession *session_;
};

class PipeWireSession
{
public:
    enum class Status
    {
        SUCCESS,
        UNINITIALIZED,
        UNAVAILABLE,
        DENIED,
        ERROR
    };
    using Callback = std::function<void(Status)>;

    PipeWireSession();
    ~PipeWireSession();

    void Init(Callback callback, int fd = kInvalidPipeWireFd);
    const std::deque<PipeWireNode::PipeWireNodePtr> &nodes() const { return nodes_; }

    friend class CameraPortalNotifier;
    friend class PipeWireNode;
    friend class VideoCaptureModulePipeWire;

private:
    void InitPipeWire(int fd);
    bool StartPipeWire(int fd);
    void StopPipeWire();
    void PipeWireSync();

    static void OnCoreError(void *data, uint32_t id, int seq, int res, const char *message);
    static void OnCoreDone(void *data, uint32_t id, int seq);

    static void OnRegistryGlobal(void *data,
                                 uint32_t id,
                                 uint32_t permissions,
                                 const char *type,
                                 uint32_t version,
                                 const spa_dict *props);
    static void OnRegistryGlobalRemove(void *data, uint32_t id);

    void Finish(Status status);
    void Cleanup();

    std::mutex callback_lock_;
    Callback callback_ OCTK_ATTRIBUTE_GUARDED_BY(&callback_lock_) = nullptr;
    Status status_;

    std::thread m_worker_thread;
    GMainLoop* m_loop = nullptr;
    GMainContext* m_context = nullptr;
    std::atomic<bool> m_running{false};

    struct pw_thread_loop *pw_main_loop_ = nullptr;
    struct pw_context *pw_context_ = nullptr;
    struct pw_core *pw_core_ = nullptr;
    struct spa_hook core_listener_;

    struct pw_registry *pw_registry_ = nullptr;
    struct spa_hook registry_listener_;

    int sync_seq_ = 0;

    std::deque<PipeWireNode::PipeWireNodePtr> nodes_;
    std::unique_ptr<portal::CameraPortal> portal_;
    std::unique_ptr<CameraPortalNotifier> portal_notifier_;
    friend class CameraCapturePipeWire;
};

OCTK_END_NAMESPACE