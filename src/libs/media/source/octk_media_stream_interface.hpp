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

// This file contains interfaces for MediaStream, MediaTrack and MediaSource.
// These interfaces are used for implementing MediaStream and MediaTrack.

#ifndef _OCTK_MEDIA_STREAM_INTERFACE_HPP
#define _OCTK_MEDIA_STREAM_INTERFACE_HPP

#include <octk_recordable_encoded_frame.hpp>
#include <octk_video_source_interface.hpp>
#include <octk_context_checker.hpp>
#include <octk_video_frame.hpp>
#include <octk_optional.hpp>
#include <octk_assert.hpp>

#include <optional>
#include <string>
#include <vector>
#include <list>

OCTK_BEGIN_NAMESPACE

// Generic observer interface.
class ObserverInterface
{
public:
    virtual void onChanged() = 0;

protected:
    virtual ~ObserverInterface() { }
};

class NotifierInterface
{
public:
    virtual void registerObserver(ObserverInterface *observer) = 0;
    virtual void unregisterObserver(ObserverInterface *observer) = 0;

    virtual ~NotifierInterface() { }
};

// Implements a template version of a notifier.
template <typename T> class Notifier : public T
{
public:
    Notifier() = default;

    virtual void registerObserver(ObserverInterface *observer)
    {
        OCTK_DCHECK_RUN_ON(&mContextChecker);
        OCTK_DCHECK(observer != nullptr);
        observers_.push_back(observer);
    }

    virtual void unregisterObserver(ObserverInterface *observer)
    {
        OCTK_DCHECK_RUN_ON(&mContextChecker);
        for (std::list<ObserverInterface *>::iterator it = observers_.begin(); it != observers_.end(); it++)
        {
            if (*it == observer)
            {
                observers_.erase(it);
                break;
            }
        }
    }

    void fireOnChanged()
    {
        OCTK_DCHECK_RUN_ON(&mContextChecker);
        // Copy the list of observers to avoid a crash if the observer object
        // unregisters as a result of the OnChanged() call. If the same list is used
        // UnregisterObserver will affect the list make the iterator invalid.
        std::list<ObserverInterface *> observers = observers_;
        for (std::list<ObserverInterface *>::iterator it = observers.begin(); it != observers.end(); ++it)
        {
            (*it)->onChanged();
        }
    }

protected:
    std::list<ObserverInterface *> observers_ OCTK_ATTRIBUTE_GUARDED_BY(mContextChecker);

private:
    OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS ContextChecker mContextChecker{ContextChecker::InitialState::kDetached};
};

// Base class for sources. A MediaStreamTrack has an underlying source that
// provides media. A source can be shared by multiple tracks.
class OCTK_MEDIA_API MediaSourceInterface : public NotifierInterface
{
public:
    enum SourceState
    {
        kInitializing,
        kLive,
        kEnded,
        kMuted
    };

    virtual SourceState state() const = 0;

    virtual bool remote() const = 0;

protected:
    ~MediaSourceInterface() override = default;
};

// C++ version of MediaStreamTrack.
// See: https://www.w3.org/TR/mediacapture-streams/#mediastreamtrack
class OCTK_MEDIA_API MediaStreamTrackInterface : public NotifierInterface
{
public:
    enum TrackState
    {
        kLive,
        kEnded,
    };

    static const char *const kAudioKind;
    static const char *const kVideoKind;

    // The kind() method must return kAudioKind only if the object is a
    // subclass of AudioTrackInterface, and kVideoKind only if the
    // object is a subclass of VideoTrackInterface. It is typically used
    // to protect a static_cast<> to the corresponding subclass.
    virtual std::string kind() const = 0;

    // Track identifier.
    virtual std::string id() const = 0;

    // A disabled track will produce silence (if audio) or black frames (if
    // video). Can be disabled and re-enabled.
    virtual bool enabled() const = 0;
    virtual bool setEnabled(bool enable) = 0;

    // Live or ended. A track will never be live again after becoming ended.
    virtual TrackState state() const = 0;

protected:
    ~MediaStreamTrackInterface() override = default;
};

// VideoTrackSourceInterface is a reference counted source used for
// VideoTracks. The same source can be used by multiple VideoTracks.
// VideoTrackSourceInterface is designed to be invoked on the signaling thread
// except for rtc::VideoSourceInterface<VideoFrame> methods that will be invoked
// on the worker thread via a VideoTrack. A custom implementation of a source
// can inherit AdaptedVideoTrackSource instead of directly implementing this
// interface.
class OCTK_MEDIA_API VideoTrackSourceInterface : public MediaSourceInterface, public VideoSourceInterface<VideoFrame>
{
public:
    struct Stats
    {
        // Original size of captured frame, before video adaptation.
        int inputWidth;
        int inputHeight;
    };

    // Indicates that parameters suitable for screencasts should be automatically
    // applied to RtpSenders.
    // TODO(perkj): Remove these once all known applications have moved to
    // explicitly setting suitable parameters for screencasts and don't need this
    // implicit behavior.
    virtual bool isScreencast() const = 0;

    // Indicates that the encoder should denoise video before encoding it.
    // If it is not set, the default configuration is used which is different
    // depending on video codec.
    // TODO(perkj): Remove this once denoising is done by the source, and not by
    // the encoder.
    virtual Optional<bool> needsDenoising() const = 0;

    // Returns false if no stats are available, e.g, for a remote source, or a
    // source which has not seen its first frame yet.
    //
    // Implementation should avoid blocking.
    virtual bool getStats(Stats *stats) = 0;

    // Returns true if encoded output can be enabled in the source.
    virtual bool isSupportsEncodedOutput() const = 0;

    // Reliably cause a key frame to be generated in encoded output.
    // TODO(bugs.webrtc.org/11115): find optimal naming.
    virtual void generateKeyFrame() = 0;

    // Add an encoded video sink to the source and additionally cause
    // a key frame to be generated from the source. The sink will be
    // invoked from a decoder queue.
    virtual void addEncodedSink(VideoSinkInterface<RecordableEncodedFrame> *sink) = 0;

    // Removes an encoded video sink from the source.
    virtual void removeEncodedSink(VideoSinkInterface<RecordableEncodedFrame> *sink) = 0;

    // Notify about constraints set on the source. The information eventually gets
    // routed to attached sinks via VideoSinkInterface<>::OnConstraintsChanged.
    // The call is expected to happen on the network thread.
    // TODO(crbug/1255737): make pure virtual once downstream project adapts.
    virtual void processConstraints(const VideoTrackSourceConstraints & /* constraints */) { }

protected:
    ~VideoTrackSourceInterface() override = default;
};

// VideoTrackInterface is designed to be invoked on the signaling thread except
// for rtc::VideoSourceInterface<VideoFrame> methods that must be invoked
// on the worker thread.
// PeerConnectionFactory::CreateVideoTrack can be used for creating a VideoTrack
// that ensures thread safety and that all methods are called on the right thread.
class OCTK_MEDIA_API VideoTrackInterface : public MediaStreamTrackInterface, public VideoSourceInterface<VideoFrame>
{
public:
    // Video track content hint, used to override the source is_screencast property.
    // See https://crbug.com/653531 and https://w3c.github.io/mst-content-hint.
    enum class ContentHint
    {
        kNone,
        kFluid,
        kDetailed,
        kText
    };

    // Register a video sink for this track. Used to connect the track to the underlying video engine.
    void addOrUpdateSink(VideoSinkInterface<VideoFrame> * /* sink */, const VideoSinkWants & /* wants */) override { }

    void removeSink(VideoSinkInterface<VideoFrame> * /* sink */) override { }

    virtual VideoTrackSourceInterface *getSource() const = 0;

    virtual ContentHint contentHint() const;

    virtual void setContentHint(ContentHint /* hint */) { }

protected:
    ~VideoTrackInterface() override = default;
};

// Interface for receiving audio data from a AudioTrack.
class AudioTrackSinkInterface
{
public:
    virtual void onData(const void * /* audio_data */,
                        int /* bits_per_sample */,
                        int /* sample_rate */,
                        size_t /* number_of_channels */,
                        size_t /* number_of_frames */)
    {
        OCTK_DCHECK_NOTREACHED() << "This method must be overridden, or not used.";
    }

    // In this method, `absolute_capture_timestamp_ms`, when available, is
    // supposed to deliver the timestamp when this audio frame was originally
    // captured. This timestamp MUST be based on the same clock as TimeMillis().
    virtual void onData(const void *audioData,
                        int bitsPerSample,
                        int sampleRate,
                        size_t numberOfChannels,
                        size_t numberOfFrames,
                        Optional<int64_t> /* absoluteCaptureTimestampMS */)
    {
        // TODO(bugs.webrtc.org/10739): Deprecate the old OnData and make this one pure virtual.
        return onData(audioData, bitsPerSample, sampleRate, numberOfChannels, numberOfFrames);
    }

    // Returns the number of channels encoded by the sink. This can be less than
    // the number_of_channels if down-mixing occur. A value of -1 means an unknown number.
    virtual int numPreferredChannels() const { return -1; }

protected:
    virtual ~AudioTrackSinkInterface() { }
};

// AudioSourceInterface is a reference counted source used for AudioTracks.
// The same source can be used by multiple AudioTracks.
class OCTK_MEDIA_API AudioSourceInterface : public MediaSourceInterface
{
public:
    class AudioObserver
    {
    public:
        virtual void onSetVolume(double volume) = 0;

    protected:
        virtual ~AudioObserver() { }
    };

    // TODO(deadbeef): Makes all the interfaces pure virtual after they're
    // implemented in chromium.

    // Sets the volume of the source. `volume` is in  the range of [0, 10].
    // TODO(tommi): This method should be on the track and ideally volume should
    // be applied in the track in a way that does not affect clones of the track.
    virtual void setVolume(double /* volume */) { }

    // Registers/unregisters observers to the audio source.
    virtual void registerAudioObserver(AudioObserver * /* observer */) { }

    virtual void unregisterAudioObserver(AudioObserver * /* observer */) { }

    // TODO(tommi): Make pure virtual.
    virtual void addSink(AudioTrackSinkInterface * /* sink */) { }

    virtual void removeSink(AudioTrackSinkInterface * /* sink */) { }

    // Returns options for the AudioSource.
    // (for some of the settings this approach is broken, e.g. setting
    // audio network adaptation on the source is the wrong layer of abstraction).
    //    virtual const cricket::AudioOptions options() const; //TODO
};

// Interface of the audio processor used by the audio track to collect
// statistics.
class OCTK_MEDIA_API AudioProcessorInterface
{
public:
    struct AudioProcessorStatistics
    {
        bool typingNoiseDetected = false;
        //        AudioProcessingStats apm_statistics; //TODO
    };

    // Get audio processor statistics. The `has_remote_tracks` argument should be
    // set if there are active remote tracks (this would usually be true during
    // a call). If there are no remote tracks some of the stats will not be set by
    // the AudioProcessor, because they only make sense if there is at least one
    // remote track.
    virtual AudioProcessorStatistics getStats(bool hasRemoteTracks) = 0;

protected:
    virtual ~AudioProcessorInterface() { }
};

class OCTK_MEDIA_API AudioTrackInterface : public MediaStreamTrackInterface
{
public:
    // TODO(deadbeef): Figure out if the following interface should be const or not.
    virtual AudioSourceInterface *getSource() const = 0;

    // Add/Remove a sink that will receive the audio data from the track.
    virtual void addSink(AudioTrackSinkInterface *sink) = 0;
    virtual void removeSink(AudioTrackSinkInterface *sink) = 0;

    // Get the signal level from the audio track.
    // Return true on success, otherwise false.
    // TODO(deadbeef): Change the interface to int GetSignalLevel() and pure
    // virtual after it's implemented in chromium.
    virtual bool getSignalLevel(int *level);

    // Get the audio processor used by the audio track. Return null if the track
    // does not have any processor.
    // TODO(deadbeef): Make the interface pure virtual.
    virtual std::shared_ptr<AudioProcessorInterface> getAudioProcessor();

protected:
    ~AudioTrackInterface() override = default;
};

typedef std::vector<std::shared_ptr<AudioTrackInterface>> AudioTrackVector;
typedef std::vector<std::shared_ptr<VideoTrackInterface>> VideoTrackVector;

// C++ version of https://www.w3.org/TR/mediacapture-streams/#mediastream.
//
// A major difference is that remote audio/video tracks (received by a
// PeerConnection/RtpReceiver) are not synchronized simply by adding them to
// the same stream; a session description with the correct "a=msid" attributes
// must be pushed down.
//
// Thus, this interface acts as simply a container for tracks.
class OCTK_MEDIA_API MediaStreamInterface : public NotifierInterface
{
public:
    virtual std::string id() const = 0;

    virtual AudioTrackVector getAudioTracks() = 0;
    virtual VideoTrackVector getVideoTracks() = 0;
    virtual std::shared_ptr<AudioTrackInterface> findAudioTrack(const std::string &trackId) = 0;
    virtual std::shared_ptr<VideoTrackInterface> findVideoTrack(const std::string &trackId) = 0;

    // Takes ownership of added tracks.
    virtual bool addTrack(std::shared_ptr<AudioTrackInterface> /* track */) = 0;
    virtual bool addTrack(std::shared_ptr<VideoTrackInterface> /* track */) = 0;
    virtual bool removeTrack(std::shared_ptr<AudioTrackInterface> /* track */) = 0;
    virtual bool removeTrack(std::shared_ptr<VideoTrackInterface> /* track */) = 0;

protected:
    ~MediaStreamInterface() override = default;
};
OCTK_END_NAMESPACE

#endif // _OCTK_MEDIA_STREAM_INTERFACE_HPP
