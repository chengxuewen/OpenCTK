//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_VIDEO_SOURCE_RESTRICTIONS_HPP
#define _OCTK_VIDEO_SOURCE_RESTRICTIONS_HPP

#include <octk_media_global.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// Describes optional restrictions to the resolution and frame rate of a video
// source.
class VideoSourceRestrictions
{
public:
    // Constructs without any restrictions.
    VideoSourceRestrictions();
    // All values must be positive or nullopt.
    // TODO(hbos): Support expressing "disable this stream"?
    VideoSourceRestrictions(Optional<size_t> max_pixels_per_frame,
                            Optional<size_t> target_pixels_per_frame,
                            Optional<double> max_frame_rate);

    bool operator==(const VideoSourceRestrictions &rhs) const
    {
        return max_pixels_per_frame_ == rhs.max_pixels_per_frame_ &&
               target_pixels_per_frame_ == rhs.target_pixels_per_frame_ && max_frame_rate_ == rhs.max_frame_rate_;
    }
    bool operator!=(const VideoSourceRestrictions &rhs) const { return !(*this == rhs); }

    std::string ToString() const;

    // The source must produce a resolution less than or equal to
    // max_pixels_per_frame().
    const Optional<size_t> &max_pixels_per_frame() const;
    // The source should produce a resolution as close to the
    // target_pixels_per_frame() as possible, provided this does not exceed
    // max_pixels_per_frame().
    // The actual pixel count selected depends on the capabilities of the source.
    // TODO(hbos): Clarify how "target" is used. One possible implementation: open
    // the camera in the smallest resolution that is greater than or equal to the
    // target and scale it down to the target if it is greater. Is this an
    // accurate description of what this does today, or do we do something else?
    const Optional<size_t> &target_pixels_per_frame() const;
    const Optional<double> &max_frame_rate() const;

    void set_max_pixels_per_frame(Optional<size_t> max_pixels_per_frame);
    void set_target_pixels_per_frame(Optional<size_t> target_pixels_per_frame);
    void set_max_frame_rate(Optional<double> max_frame_rate);

    // Update `this` with min(`this`, `other`).
    void UpdateMin(const VideoSourceRestrictions &other);

private:
    // These map to rtc::VideoSinkWants's `max_pixel_count` and
    // `target_pixel_count`.
    Optional<size_t> max_pixels_per_frame_;
    Optional<size_t> target_pixels_per_frame_;
    Optional<double> max_frame_rate_;
};

bool DidRestrictionsIncrease(VideoSourceRestrictions before, VideoSourceRestrictions after);
bool DidRestrictionsDecrease(VideoSourceRestrictions before, VideoSourceRestrictions after);
bool DidIncreaseResolution(VideoSourceRestrictions restrictions_before, VideoSourceRestrictions restrictions_after);
bool DidDecreaseResolution(VideoSourceRestrictions restrictions_before, VideoSourceRestrictions restrictions_after);
bool DidIncreaseFrameRate(VideoSourceRestrictions restrictions_before, VideoSourceRestrictions restrictions_after);
bool DidDecreaseFrameRate(VideoSourceRestrictions restrictions_before, VideoSourceRestrictions restrictions_after);

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_SOURCE_RESTRICTIONS_HPP
