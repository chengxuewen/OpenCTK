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

#ifndef _OCTK_METRICS_HPP
#define _OCTK_METRICS_HPP

#include <octk_string_utils.hpp>
#include <octk_string_view.hpp>
#include <octk_checks.hpp>

#include <cstddef>
#include <atomic>
#include <string>
#include <memory>
#include <map>

#if defined(OCTK_DISABLE_METRICS)
#   define OCTK_METRICS_ENABLED 0
#else
#   define OCTK_METRICS_ENABLED 1
#endif

OCTK_BEGIN_NAMESPACE

namespace metrics_impl
{
template <typename... Ts>
void NoOp(const Ts &...) {}
}  // namespace metrics_impl

#if OCTK_METRICS_ENABLED
#   define EXPECT_METRIC_EQ(val1, val2) EXPECT_EQ(val1, val2)
#   define EXPECT_METRIC_EQ_WAIT(val1, val2, timeout) \
    EXPECT_EQ_WAIT(val1, val2, timeout)
#   define EXPECT_METRIC_GT(val1, val2) EXPECT_GT(val1, val2)
#   define EXPECT_METRIC_LE(val1, val2) EXPECT_LE(val1, val2)
#   define EXPECT_METRIC_TRUE(conditon) EXPECT_TRUE(conditon)
#   define EXPECT_METRIC_FALSE(conditon) EXPECT_FALSE(conditon)
#   define EXPECT_METRIC_THAT(value, matcher) EXPECT_THAT(value, matcher)
#else
#   define EXPECT_METRIC_EQ(val1, val2) octk::metrics_impl::NoOp(val1, val2)
#   define EXPECT_METRIC_EQ_WAIT(val1, val2, timeout) \
    octk::metrics_impl::NoOp(val1, val2, timeout)
#   define EXPECT_METRIC_GT(val1, val2) octk::metrics_impl::NoOp(val1, val2)
#   define EXPECT_METRIC_LE(val1, val2) octk::metrics_impl::NoOp(val1, val2)
#   define EXPECT_METRIC_TRUE(condition) \
    octk::metrics_impl::NoOp(condition || true)
#   define EXPECT_METRIC_FALSE(condition) \
    octk::metrics_impl::NoOp(condition && false)
#   define EXPECT_METRIC_THAT(value, matcher) \
    octk::metrics_impl::NoOp(value, testing::_)
#endif

#if OCTK_METRICS_ENABLED
// Macros for allowing apps (e.g. Chrome) to gather and aggregate statistics.
//
// Histogram for counters.
// OCTK_HISTOGRAM_COUNTS(name, sample, min, max, bucket_count);
//
// Histogram for enumerators.
// The boundary should be above the max enumerator sample.
// OCTK_HISTOGRAM_ENUMERATION(name, sample, boundary);
//
//
// The macros use the methods HistogramFactoryGetCounts,
// HistogramFactoryGetEnumeration and HistogramAdd.
//
// By default kit provides implementations of the aforementioned methods
// that can be found in system_wrappers/source/metrics.cc. If clients want to
// provide a custom version, they will have to:
//
// 1. Compile kit defining the preprocessor macro
//    WEBOCTK_EXCLUDE_METRICS_DEFAULT (if GN is used this can be achieved
//    by setting the GN arg rtc_exclude_metrics_default to true).
// 2. Provide implementations of:
//    Histogram* octk::metrics::HistogramFactoryGetCounts(StringView name, int sample, int min, int max, int bucket_count);
//    Histogram* octk::metrics::HistogramFactoryGetEnumeration(StringView name, int sample, int boundary);
//    void octk::metrics::HistogramAdd(Histogram* histogram_pointer, StringView name, int sample);
//
// Example usage:
//
// OCTK_HISTOGRAM_COUNTS("RTC.Video.NacksSent", nacks_sent, 1, 100000, 100);
//
// enum Types {
//   kTypeX,
//   kTypeY,
//   kBoundary,
// };
//
// OCTK_HISTOGRAM_ENUMERATION("RTC.Types", kTypeX, kBoundary);
//
// NOTE: It is recommended to do the Chromium review for modifications to
// histograms.xml before new metrics are committed to WebRTC.

// Macros for adding samples to a named histogram.

// Histogram for counters (exponentially spaced buckets).
#define OCTK_HISTOGRAM_COUNTS_100(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 100, 50)
#define OCTK_HISTOGRAM_COUNTS_200(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 200, 50)
#define OCTK_HISTOGRAM_COUNTS_500(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 500, 50)
#define OCTK_HISTOGRAM_COUNTS_1000(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 1000, 50)
#define OCTK_HISTOGRAM_COUNTS_10000(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 10000, 50)
#define OCTK_HISTOGRAM_COUNTS_100000(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 100000, 50)
#define OCTK_HISTOGRAM_COUNTS_1M(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 1000000, 50)
#define OCTK_HISTOGRAM_COUNTS_1G(name, sample) OCTK_HISTOGRAM_COUNTS(name, sample, 1, 1000000000, 50)
#define OCTK_HISTOGRAM_COUNTS(name, sample, min, max, bucket_count) \
    OCTK_HISTOGRAM_COMMON_BLOCK(name, sample, \
                                octk::metrics::HistogramFactoryGetCounts( \
                                name, min, max, bucket_count))

#define OCTK_HISTOGRAM_COUNTS_LINEAR(name, sample, min, max, bucket_count) \
    OCTK_HISTOGRAM_COMMON_BLOCK(name, sample, \
                                octk::metrics::HistogramFactoryGetCountsLinear( \
                                name, min, max, bucket_count))

// Slow metrics: pointer to metric is acquired at each call and is not cached.
#define OCTK_HISTOGRAM_COUNTS_SPARSE_100(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 100, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_200(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 200, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_500(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 500, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_1000(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 1000, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_10000(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 10000, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_100000(name, sample) OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, 1, 100000, 50)
#define OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, min, max, bucket_count) \
  OCTK_HISTOGRAM_COMMON_BLOCK_SLOW(name, sample, \
                                    octk::metrics::HistogramFactoryGetCounts( \
                                    name, min, max, bucket_count))

// Histogram for percentage (evenly spaced buckets).
#define OCTK_HISTOGRAM_PERCENTAGE_SPARSE(name, sample) OCTK_HISTOGRAM_ENUMERATION_SPARSE(name, sample, 101)

// Histogram for booleans.
#define OCTK_HISTOGRAM_BOOLEAN_SPARSE(name, sample) OCTK_HISTOGRAM_ENUMERATION_SPARSE(name, sample, 2)

// Histogram for enumerators (evenly spaced buckets).
// `boundary` should be above the max enumerator sample.
//
// TODO(qingsi): Refactor the default implementation given by RtcHistogram,
// which is already sparse, and remove the boundary argument from the macro.
#define OCTK_HISTOGRAM_ENUMERATION_SPARSE(name, sample, boundary) \
    OCTK_HISTOGRAM_COMMON_BLOCK_SLOW( \
        name, sample, \
        octk::metrics::SparseHistogramFactoryGetEnumeration(name, boundary))

// Histogram for percentage (evenly spaced buckets).
#define OCTK_HISTOGRAM_PERCENTAGE(name, sample) OCTK_HISTOGRAM_ENUMERATION(name, sample, 101)

// Histogram for booleans.
#define OCTK_HISTOGRAM_BOOLEAN(name, sample) OCTK_HISTOGRAM_ENUMERATION(name, sample, 2)

// Histogram for enumerators (evenly spaced buckets).
// `boundary` should be above the max enumerator sample.
#define OCTK_HISTOGRAM_ENUMERATION(name, sample, boundary) \
    OCTK_HISTOGRAM_COMMON_BLOCK_SLOW( \
        name, sample, \
        octk::metrics::HistogramFactoryGetEnumeration(name, boundary))

// The name of the histogram should not vary.
#define OCTK_HISTOGRAM_COMMON_BLOCK(constant_name, sample, factory_get_invocation) \
    do { \
        static std::atomic<octk::metrics::Histogram*> atomic_histogram_pointer(nullptr); \
        octk::metrics::Histogram* histogram_pointer = atomic_histogram_pointer.load(std::memory_order_acquire); \
        if (!histogram_pointer) { \
            histogram_pointer = factory_get_invocation; \
            octk::metrics::Histogram* null_histogram = nullptr; \
            atomic_histogram_pointer.compare_exchange_strong(null_histogram, histogram_pointer); \
        } \
        if (histogram_pointer) { \
            octk::metrics::HistogramAdd(histogram_pointer, sample); \
        } \
  } while (0)

// The histogram is constructed/found for each call.
// May be used for histograms with infrequent updates.`
#define OCTK_HISTOGRAM_COMMON_BLOCK_SLOW(name, sample, factory_get_invocation) \
    do { \
        octk::metrics::Histogram* histogram_pointer = factory_get_invocation; \
        if (histogram_pointer) { \
            octk::metrics::HistogramAdd(histogram_pointer, sample); \
        } \
    } while (0)

// Helper macros.
// Macros for calling a histogram with varying name (e.g. when using a metric
// in different modes such as real-time vs screenshare). Fast, because pointer
// is cached. `index` should be different for different names. Allowed `index`
// values are 0, 1, and 2.
#define OCTK_HISTOGRAMS_COUNTS_100(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 100, 50))

#define OCTK_HISTOGRAMS_COUNTS_200(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 200, 50))

#define OCTK_HISTOGRAMS_COUNTS_500(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 500, 50))

#define OCTK_HISTOGRAMS_COUNTS_1000(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 1000, 50))

#define OCTK_HISTOGRAMS_COUNTS_10000(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 10000, 50))

#define OCTK_HISTOGRAMS_COUNTS_100000(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_COUNTS(name, sample, 1, 100000, 50))

#define OCTK_HISTOGRAMS_ENUMERATION(index, name, sample, boundary) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_ENUMERATION(name, sample, boundary))

#define OCTK_HISTOGRAMS_PERCENTAGE(index, name, sample) \
    OCTK_HISTOGRAMS_COMMON(index, name, sample, OCTK_HISTOGRAM_PERCENTAGE(name, sample))

#define OCTK_HISTOGRAMS_COMMON(index, name, sample, macro_invocation) \
    do {                                                               \
        switch (index) {                                                 \
        case 0:                                                        \
            macro_invocation;                                            \
            break;                                                       \
        case 1:                                                        \
            macro_invocation;                                            \
            break;                                                       \
        case 2:                                                        \
            macro_invocation;                                            \
            break;                                                       \
        default:                                                       \
            OCTK_DCHECK_NOTREACHED();                                     \
        }                                                                \
    } while (0)

#else

// This section defines no-op alternatives to the metrics macros when OCTK_METRICS_ENABLED is defined.
#define OCTK_HISTOGRAM_COUNTS_100(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_200(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_500(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_1000(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_10000(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_100000(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_1M(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_1G(name, sample) octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS(name, sample, min, max, bucket_count) \
    octk::metrics_impl::NoOp(name, sample, min, max, bucket_count)
#define OCTK_HISTOGRAM_COUNTS_LINEAR(name, sample, min, max, bucket_count) \
    octk::metrics_impl::NoOp(name, sample, min, max, bucket_count)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_100(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_200(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_500(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_1000(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_10000(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE_100000(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_COUNTS_SPARSE(name, sample, min, max, bucket_count) \
    octk::metrics_impl::NoOp(name, sample, min, max, bucket_count)
#define OCTK_HISTOGRAM_PERCENTAGE_SPARSE(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_BOOLEAN_SPARSE(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_ENUMERATION_SPARSE(name, sample, boundary) \
    octk::metrics_impl::NoOp(name, sample, boundary)
#define OCTK_HISTOGRAM_PERCENTAGE(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_BOOLEAN(name, sample) \
    octk::metrics_impl::NoOp(name, sample)
#define OCTK_HISTOGRAM_ENUMERATION(name, sample, boundary) \
    octk::metrics_impl::NoOp(name, sample, boundary)
#define OCTK_HISTOGRAM_COMMON_BLOCK(constant_name, sample, factory_get_invocation) \
    octk::metrics_impl::NoOp(constant_name, sample, factory_get_invocation)
#define OCTK_HISTOGRAM_COMMON_BLOCK_SLOW(name, sample, factory_get_invocation) \
    octk::metrics_impl::NoOp(name, sample, factory_get_invocation)
#define OCTK_HISTOGRAMS_COUNTS_100(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COUNTS_200(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COUNTS_500(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COUNTS_1000(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COUNTS_10000(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COUNTS_100000(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_ENUMERATION(index, name, sample, boundary) \
    octk::metrics_impl::NoOp(index, name, sample, boundary)
#define OCTK_HISTOGRAMS_PERCENTAGE(index, name, sample) \
    octk::metrics_impl::NoOp(index, name, sample)
#define OCTK_HISTOGRAMS_COMMON(index, name, sample, macro_invocation) \
    octk::metrics_impl::NoOp(index, name, sample, macro_invocation)
#endif  // OCTK_METRICS_ENABLED

namespace metrics
{

// Time that should have elapsed for stats that are gathered once per call.
constexpr int kMinRunTimeInSeconds = 10;

class Histogram;

// Functions for getting pointer to histogram (constructs or finds the named
// histogram).

// Get histogram for counters.
Histogram *HistogramFactoryGetCounts(StringView name,
                                     int min,
                                     int max,
                                     int bucket_count);

// Get histogram for counters with linear bucket spacing.
Histogram *HistogramFactoryGetCountsLinear(StringView name,
                                           int min,
                                           int max,
                                           int bucket_count);

// Get histogram for enumerators.
// `boundary` should be above the max enumerator sample.
Histogram *HistogramFactoryGetEnumeration(StringView name, int boundary);

// Get sparse histogram for enumerators.
// `boundary` should be above the max enumerator sample.
Histogram *SparseHistogramFactoryGetEnumeration(StringView name,
                                                int boundary);

// Function for adding a `sample` to a histogram.
void HistogramAdd(Histogram *histogram_pointer, int sample);

struct SampleInfo
{
    SampleInfo(StringView name, int min, int max, size_t bucket_count);
    ~SampleInfo();

    const std::string name;
    const int min;
    const int max;
    const size_t bucket_count;
    std::map<int, int> samples;  // <value, # of events>
};

// Enables collection of samples.
// This method should be called before any other call into webrtc.
void Enable();

// Gets histograms and clears all samples.
void GetAndReset(std::map<std::string, std::unique_ptr<SampleInfo>, StringViewCmp> *histograms);

// Functions below are mainly for testing.

// Clears all samples.
void Reset();

// Returns the number of times the `sample` has been added to the histogram.
int NumEvents(StringView name, int sample);

// Returns the total number of added samples to the histogram.
int NumSamples(StringView name);

// Returns the minimum sample value (or -1 if the histogram has no samples).
int MinSample(StringView name);

// Returns a map with keys the samples with at least one event and values the
// number of events for that sample.
std::map<int, int> Samples(StringView name);
}  // namespace metrics

OCTK_END_NAMESPACE

#endif // _OCTK_METRICS_HPP
