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

#ifndef _OCTK_SOCKET_HPP
#define _OCTK_SOCKET_HPP

#include <octk_task_event.hpp>

OCTK_BEGIN_NAMESPACE

// General interface for the socket implementations of various networks.  The
// methods match those of normal UNIX sockets very closely.
class OCTK_CORE_API Socket
{
public:
    // struct ReceiveBuffer {
    //     ReceiveBuffer(Buffer & payload) : payload(payload)
    //     {}
    //
    //     Optional<Timestamp> arrival_time;
    //     SocketAddress source_address;
    //     EcnMarking ecn = EcnMarking::kNotEct;
    //     Buffer &payload;
    // };
    virtual ~Socket() {}

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;
    //
    // // Returns the address to which the socket is bound.  If the socket is not
    // // bound, then the any-address is returned.
    // virtual SocketAddress GetLocalAddress() const = 0;
    //
    // // Returns the address to which the socket is connected.  If the socket is
    // // not connected, then the any-address is returned.
    // virtual SocketAddress GetRemoteAddress() const = 0;
    //
    // virtual int Bind(const SocketAddress &addr) = 0;
    // virtual int Connect(const SocketAddress &addr) = 0;
    // virtual int Send(const void *pv, size_t cb) = 0;
    // virtual int SendTo(const void *pv, size_t cb, const SocketAddress &addr) = 0;
    // // `timestamp` is in units of microseconds.
    // virtual int Recv(void *pv, size_t cb, int64_t *timestamp) = 0;
    // // TODO(webrtc:15368): Deprecate and remove.
    // virtual int RecvFrom(void * /* pv */,
    //                      size_t /* cb */,
    //                      SocketAddress * /* paddr */,
    //                      int64_t * /* timestamp */)
    // {
    //     // Not implemented. Use RecvFrom(ReceiveBuffer& buffer).
    //     OCTK_CHECK_NOTREACHED();
    // }
    // // Intended to replace RecvFrom(void* ...).
    // // Default implementation calls RecvFrom(void* ...) with 64Kbyte buffer.
    // // Returns number of bytes received or a negative value on error.
    // virtual int RecvFrom(ReceiveBuffer &buffer);
    // virtual int Listen(int backlog) = 0;
    // virtual Socket *Accept(SocketAddress *paddr) = 0;
    // virtual int Close() = 0;
    // virtual int GetError() const = 0;
    // virtual void SetError(int error) = 0;
    // inline bool IsBlocking() const { return IsBlockingError(GetError()); }

    // enum ConnState { CS_CLOSED, CS_CONNECTING, CS_CONNECTED };
    // virtual ConnState GetState() const = 0;
    //
    // enum Option
    // {
    //     OPT_DONTFRAGMENT,
    //     OPT_RCVBUF,                // receive buffer size
    //     OPT_SNDBUF,                // send buffer size
    //     OPT_NODELAY,               // whether Nagle algorithm is enabled
    //     OPT_IPV6_V6ONLY,           // Whether the socket is IPv6 only.
    //     OPT_DSCP,                  // DSCP code
    //     OPT_RTP_SENDTIME_EXTN_ID,  // This is a non-traditional socket option param.
    //     // This is specific to libjingle and will be used
    //     // if SendTime option is needed at socket level.
    //     OPT_SEND_ECN,              // 2-bit ECN
    //     OPT_RECV_ECN,
    //     OPT_KEEPALIVE,         // Enable socket keep alive
    //     OPT_TCP_KEEPCNT,       // Set TCP keep alive count
    //     OPT_TCP_KEEPIDLE,      // Set TCP keep alive idle time in seconds
    //     OPT_TCP_KEEPINTVL,     // Set TCP keep alive interval in seconds
    //     OPT_TCP_USER_TIMEOUT,  // Set TCP user timeout
    // };
    // virtual int GetOption(Option opt, int *value) = 0;
    // virtual int SetOption(Option opt, int value) = 0;

    // SignalReadEvent and SignalWriteEvent use multi_threaded_local to allow
    // access concurrently from different thread.
    // For example SignalReadEvent::connect will be called in AsyncUDPSocket ctor
    // but at the same time the SocketDispatcher may be signaling the read event.
    // ready to read
    // sigslot::signal1<Socket *, sigslot::multi_threaded_local> SignalReadEvent;
    // // ready to write
    // sigslot::signal1<Socket *, sigslot::multi_threaded_local> SignalWriteEvent;
    // sigslot::signal1<Socket *> SignalConnectEvent;     // connected
    // sigslot::signal2<Socket *, int> SignalCloseEvent;  // closed

protected:
    Socket() {}
};

class SocketFactory
{
public:
    virtual ~SocketFactory() {}

    // Returns a new socket.  The type can be SOCK_DGRAM and SOCK_STREAM.
    virtual Socket *CreateSocket(int family, int type) = 0;
};

class TaskThread;

// Provides the ability to wait for activity on a set of sockets.  The Thread
// class provides a nice wrapper on a socket server.
//
// The server is also a socket factory.  The sockets it creates will be
// notified of asynchronous I/O from this server's Wait method.
class SocketServer : public SocketFactory
{
public:
    static TimeDelta foreverDuration() { return Event::foreverDuration(); }

    // static std::unique_ptr<SocketServer> CreateDefault();

    // When the socket server is installed into a Thread, this function is called
    // to allow the socket server to use the thread's message queue for any
    // messaging that it might need to perform. It is also called with a null
    // argument before the thread is destroyed.
    virtual void SetMessageQueue(TaskThread * /* queue */) {}

    // Sleeps until:
    //  1) `max_wait_duration` has elapsed (unless `max_wait_duration` ==
    //  `kForever`)
    // 2) WakeUp() is called
    // While sleeping, I/O is performed if process_io is true.
    virtual bool Wait(TimeDelta max_wait_duration, bool process_io) = 0;

    // Causes the current wait (if one is in progress) to wake up.
    virtual void WakeUp() = 0;

//     // A network binder will bind the created sockets to a network.
//     // It is only used in PhysicalSocketServer.
//     void set_network_binder(NetworkBinderInterface *binder)
//     {
//         network_binder_ = binder;
//     }
//     NetworkBinderInterface *network_binder() const { return network_binder_; }
//
// private:
//     NetworkBinderInterface *network_binder_ = nullptr;
};

class OCTK_CORE_API NullSocketServer : public SocketServer
{
public:
    NullSocketServer();
    ~NullSocketServer() override;

    bool Wait(TimeDelta max_wait_duration, bool process_io) override;
    void WakeUp() override;

    Socket *CreateSocket(int family, int type) override;

private:
    Event event_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_SOCKET_HPP
