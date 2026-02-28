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

#include <private/octk_portal_request_response_p.hpp>
#include <octk_string_view.hpp>

#include <gio/gio.h>
#include <stdint.h>

#include <string>

OCTK_BEGIN_NAMESPACE

namespace portal
{
namespace xdg
{

constexpr char kDesktopBusName[] = "org.freedesktop.portal.Desktop";
constexpr char kDesktopObjectPath[] = "/org/freedesktop/portal/desktop";
constexpr char kDesktopRequestObjectPath[] = "/org/freedesktop/portal/desktop/request";
constexpr char kSessionInterfaceName[] = "org.freedesktop.portal.Session";
constexpr char kRequestInterfaceName[] = "org.freedesktop.portal.Request";
constexpr char kScreenCastInterfaceName[] = "org.freedesktop.portal.ScreenCast";

using ProxyRequestCallback = void (*)(GObject *, GAsyncResult *, gpointer);
using SessionRequestCallback = void (*)(GDBusProxy *, GAsyncResult *, gpointer);
using SessionRequestResponseSignalHandler =
    void (*)(GDBusConnection *, const char *, const char *, const char *, const char *, GVariant *, gpointer);
using StartRequestResponseSignalHandler =
    void (*)(GDBusConnection *, const char *, const char *, const char *, const char *, GVariant *, gpointer);
using SessionStartRequestedHandler = void (*)(GDBusProxy *, GAsyncResult *, gpointer);

OCTK_MEDIA_API std::string RequestResponseToString(RequestResponse request);

RequestResponse RequestResponseFromPortalResponse(uint32_t portal_response);

// Returns a string path for signal handle based on the provided connection and token.
OCTK_MEDIA_API std::string PrepareSignalHandle(StringView token, GDBusConnection *connection);

// Sets up the callback to execute when a response signal is received for the given object.
OCTK_MEDIA_API uint32_t SetupRequestResponseSignal(StringView object_path,
                                                   const GDBusSignalCallback callback,
                                                   gpointer user_data,
                                                   GDBusConnection *connection);

OCTK_MEDIA_API void RequestSessionProxy(StringView interface_name,
                                        const ProxyRequestCallback proxy_request_callback,
                                        GCancellable *cancellable,
                                        gpointer user_data);

OCTK_MEDIA_API void SetupSessionRequestHandlers(
    StringView portal_prefix,
    const SessionRequestCallback session_request_callback,
    const SessionRequestResponseSignalHandler request_response_signale_handler,
    GDBusConnection *connection,
    GDBusProxy *proxy,
    GCancellable *cancellable,
    std::string &portal_handle,
    guint &session_request_signal_id,
    gpointer user_data);

OCTK_MEDIA_API void StartSessionRequest(StringView prefix,
                                        StringView session_handle,
                                        const StartRequestResponseSignalHandler signal_handler,
                                        const SessionStartRequestedHandler session_started_handler,
                                        GDBusProxy *proxy,
                                        GDBusConnection *connection,
                                        GCancellable *cancellable,
                                        guint &start_request_signal_id,
                                        std::string &start_handle,
                                        gpointer user_data);

// Tears down the portal session and cleans up related objects.
OCTK_MEDIA_API void TearDownSession(StringView session_handle,
                                    GDBusProxy *proxy,
                                    GCancellable *cancellable,
                                    GDBusConnection *connection);
} // namespace xdg
} // namespace portal

OCTK_END_NAMESPACE