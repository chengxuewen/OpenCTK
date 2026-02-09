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

#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

/**
 * The RtcDataChannelInit struct represents the configuration options for a
 * WebRTC data channel. These options include whether the channel is ordered and
 * reliable, the maximum retransmit time and number of retransmits, the protocol
 * to use (sctp or quic), whether the channel is negotiated, and the channel ID.
 */
struct RtcDataChannelInit
{
    bool ordered = true;
    bool reliable = true;
    int maxRetransmitTime = -1;
    int maxRetransmits = -1;
    String protocol = {"sctp"}; // sctp | quic
    bool negotiated = false;
    int id = 0;
};

/**
 * The RtcDataChannel class represents a data channel in WebRTC.
 * Data channels are used to transmit non-audio/video data over a WebRTC peer
 * connection. This class provides a base interface for data channels to
 * implement, allowing them to be used with WebRTC's data channel mechanisms.
 */
class RtcDataChannel
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcDataChannel)

    /**
 * The RTCDataChannelState enum represents the possible states of a WebRTC data
 * channel. Data channels are used to transmit non-audio/video data over a
 * WebRTC peer connection. The possible states are: connecting, open, closing,
 * and closed.
 */
    enum class State
    {
        kConnecting,
        kOpen,
        kClosing,
        kClosed,
    };

    /**
   * The RTCDataChannelObserver class is an interface for receiving events related
   * to a WebRTC data channel. These events include changes in the channel's state
   * and incoming messages.
   */
    class Observer
    {
    public:
        /**
   * Called when the state of the data channel changes.
   * The new state is passed as a parameter.
   */
        virtual void OnStateChange(State state) = 0;

        /**
   * Called when a message is received on the data channel.
   * The message buffer, its length, and a boolean indicating whether the
   * message is binary are passed as parameters.
   */
        virtual void OnMessage(const char *buffer, int length, bool binary) = 0;

    protected:
        /**
   * The destructor for the RTCDataChannelObserver class.
   */
        virtual ~Observer() = default;
    };

    /**
   * Sends data over the data channel.
   * The data buffer, its size, and a boolean indicating whether the data is
   * binary are passed as parameters.
   */
    virtual void send(const uint8_t *data, uint32_t size, bool binary = false) = 0;

    /**
   * Registers an observer for events related to the data channel.
   * The observer object is passed as a parameter.
   */
    virtual void registerObserver(Observer *observer) = 0;

    /**
   * Returns the amount of data buffered in the data channel.
   *
   * @return uint64_t
   */
    virtual uint64_t buffered_amount() const = 0;

    /**
   * Unregisters the current observer for the data channel.
   */
    virtual void unregisterObserver() = 0;

    /**
   * Returns the label of the data channel.
   */
    virtual String label() const = 0;

    /**
   * Returns the ID of the data channel.
   */
    virtual int id() const = 0;

    /**
   * Returns the state of the data channel.
   */
    virtual State state() = 0;

    /**
   * Closes the data channel.
   */
    virtual void close() = 0;

protected:
    virtual ~RtcDataChannel() = default;
};

OCTK_END_NAMESPACE
