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

#include <private/octk_scoped_glib_p.hpp>
#include <private/octk_camera_portal_p.hpp>
#include <private/octk_pipewire_utils_p.hpp>
#include <private/octk_xdg_desktop_portal_utils_p.hpp>

#include <gio/gio.h>
#include <gio/gunixfdlist.h>

#include <mutex>

OCTK_BEGIN_NAMESPACE

namespace portal
{

using xdg::RequestResponse;
using xdg::RequestSessionProxy;
using xdg::RequestResponseFromPortalResponse;

constexpr char kCameraInterfaceName[] = "org.freedesktop.portal.Camera";

class CameraPortalPrivate
{
public:
    explicit CameraPortalPrivate(CameraPortal::PortalNotifier *notifier);
    ~CameraPortalPrivate();

    void Start();

private:
    void OnPortalDone(RequestResponse result, int fd = kInvalidPipeWireFd);

    static void OnProxyRequested(GObject *object, GAsyncResult *result, gpointer user_data);
    void ProxyRequested(GDBusProxy *proxy);

    static void OnAccessResponse(GDBusProxy *proxy, GAsyncResult *result, gpointer user_data);
    static void OnResponseSignalEmitted(GDBusConnection *connection,
                                        const char *sender_name,
                                        const char *object_path,
                                        const char *interface_name,
                                        const char *signal_name,
                                        GVariant *parameters,
                                        gpointer user_data);
    static void OnOpenResponse(GDBusProxy *proxy, GAsyncResult *result, gpointer user_data);

    std::mutex notifier_lock_;
    CameraPortal::PortalNotifier *notifier_ OCTK_ATTRIBUTE_GUARDED_BY(&notifier_lock_) = nullptr;

    GDBusConnection *connection_ = nullptr;
    GDBusProxy *proxy_ = nullptr;
    GCancellable *cancellable_ = nullptr;
    guint access_request_signal_id_ = 0;
};

CameraPortalPrivate::CameraPortalPrivate(CameraPortal::PortalNotifier *notifier)
    : notifier_(notifier)
{
}

CameraPortalPrivate::~CameraPortalPrivate()
{
    {
        std::lock_guard<std::mutex> lock(notifier_lock_);
        notifier_ = nullptr;
    }

    if (access_request_signal_id_)
    {
        g_dbus_connection_signal_unsubscribe(connection_, access_request_signal_id_);
        access_request_signal_id_ = 0;
    }
    if (cancellable_)
    {
        g_cancellable_cancel(cancellable_);
        g_object_unref(cancellable_);
        cancellable_ = nullptr;
    }
    if (proxy_)
    {
        g_object_unref(proxy_);
        proxy_ = nullptr;
        connection_ = nullptr;
    }
}

void CameraPortalPrivate::Start()
{
    cancellable_ = g_cancellable_new();
    Scoped<GError> error;
    RequestSessionProxy(kCameraInterfaceName, OnProxyRequested, cancellable_, this);
}

// static
void CameraPortalPrivate::OnProxyRequested(GObject *gobject, GAsyncResult *result, gpointer user_data)
{
    CameraPortalPrivate *that = static_cast<CameraPortalPrivate *>(user_data);
    Scoped<GError> error;
    GDBusProxy *proxy = g_dbus_proxy_new_finish(result, error.receive());
    if (!proxy)
    {
        // Ignore the error caused by user cancelling the request via `cancellable_`
        if (g_error_matches(error.get(), G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            return;
        }
        OCTK_ERROR() << "Failed to get a proxy for the portal: " << error->message;
        that->OnPortalDone(RequestResponse::kError);
        return;
    }

    OCTK_TRACE() << "Successfully created proxy for the portal.";
    that->ProxyRequested(proxy);
}

void CameraPortalPrivate::ProxyRequested(GDBusProxy *proxy)
{
    GVariantBuilder builder;
    Scoped<char> variant_string;
    std::string access_handle;

    proxy_ = proxy;
    connection_ = g_dbus_proxy_get_connection(proxy);

    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);
    variant_string = g_strdup_printf("capture%d", g_random_int_range(0, G_MAXINT));
    g_variant_builder_add(&builder, "{sv}", "handle_token", g_variant_new_string(variant_string.get()));

    access_handle = xdg::PrepareSignalHandle(variant_string.get(), connection_);
    access_request_signal_id_ = xdg::SetupRequestResponseSignal(access_handle.c_str(),
                                                                OnResponseSignalEmitted,
                                                                this,
                                                                connection_);

    OCTK_TRACE() << "Requesting camera access from the portal.";
    g_dbus_proxy_call(proxy_,
                      "AccessCamera",
                      g_variant_new("(a{sv})", &builder),
                      G_DBUS_CALL_FLAGS_NONE,
                      /*timeout_msec=*/-1,
                      cancellable_,
                      reinterpret_cast<GAsyncReadyCallback>(OnAccessResponse),
                      this);
}

// static
void CameraPortalPrivate::OnAccessResponse(GDBusProxy *proxy, GAsyncResult *result, gpointer user_data)
{
    CameraPortalPrivate *that = static_cast<CameraPortalPrivate *>(user_data);
    OCTK_DCHECK(that);

    Scoped<GError> error;
    Scoped<GVariant> variant(g_dbus_proxy_call_finish(proxy, result, error.receive()));
    if (!variant)
    {
        if (g_error_matches(error.get(), G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            return;
        }
        OCTK_ERROR() << "Failed to access portal:" << error->message;
        if (that->access_request_signal_id_)
        {
            g_dbus_connection_signal_unsubscribe(that->connection_, that->access_request_signal_id_);
            that->access_request_signal_id_ = 0;
        }
        that->OnPortalDone(RequestResponse::kError);
    }
}

// static
void CameraPortalPrivate::OnResponseSignalEmitted(GDBusConnection *connection,
                                                  const char *sender_name,
                                                  const char *object_path,
                                                  const char *interface_name,
                                                  const char *signal_name,
                                                  GVariant *parameters,
                                                  gpointer user_data)
{
    CameraPortalPrivate *that = static_cast<CameraPortalPrivate *>(user_data);
    OCTK_DCHECK(that);

    uint32_t portal_response;
    g_variant_get(parameters, "(u@a{sv})", &portal_response, nullptr);
    if (portal_response)
    {
        OCTK_INFO() << "Camera access denied by the XDG portal.";
        that->OnPortalDone(RequestResponseFromPortalResponse(portal_response));
        return;
    }

    OCTK_TRACE() << "Camera access granted by the XDG portal.";

    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

    g_dbus_proxy_call(that->proxy_,
                      "OpenPipeWireRemote",
                      g_variant_new("(a{sv})", &builder),
                      G_DBUS_CALL_FLAGS_NONE,
                      /*timeout_msec=*/-1,
                      that->cancellable_,
                      reinterpret_cast<GAsyncReadyCallback>(OnOpenResponse),
                      that);
}

void CameraPortalPrivate::OnOpenResponse(GDBusProxy *proxy, GAsyncResult *result, gpointer user_data)
{
    CameraPortalPrivate *that = static_cast<CameraPortalPrivate *>(user_data);
    OCTK_DCHECK(that);

    Scoped<GError> error;
    Scoped<GUnixFDList> outlist;
    Scoped<GVariant> variant(
        g_dbus_proxy_call_with_unix_fd_list_finish(proxy, outlist.receive(), result, error.receive()));
    if (!variant)
    {
        if (g_error_matches(error.get(), G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            return;
        }
        OCTK_ERROR() << "Failed to open PipeWire remote:" << error->message;
        if (that->access_request_signal_id_)
        {
            g_dbus_connection_signal_unsubscribe(that->connection_, that->access_request_signal_id_);
            that->access_request_signal_id_ = 0;
        }
        that->OnPortalDone(RequestResponse::kError);
        return;
    }

    int32_t index;
    g_variant_get(variant.get(), "(h)", &index);

    int fd = g_unix_fd_list_get(outlist.get(), index, error.receive());

    if (fd == kInvalidPipeWireFd)
    {
        OCTK_ERROR() << "Failed to get file descriptor from the list: " << error->message;
        that->OnPortalDone(RequestResponse::kError);
        return;
    }

    that->OnPortalDone(RequestResponse::kSuccess, fd);
}

void CameraPortalPrivate::OnPortalDone(RequestResponse result, int fd)
{
    std::lock_guard<std::mutex> lock(notifier_lock_);
    if (notifier_)
    {
        notifier_->OnCameraRequestResult(result, fd);
        notifier_ = nullptr;
    }
}

CameraPortal::CameraPortal(PortalNotifier *notifier)
    : private_(std::make_unique<CameraPortalPrivate>(notifier))
{
}

CameraPortal::~CameraPortal()
{
}

void CameraPortal::Start()
{
    private_->Start();
}

} // namespace portal

OCTK_END_NAMESPACE