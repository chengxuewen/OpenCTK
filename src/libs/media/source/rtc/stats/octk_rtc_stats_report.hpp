/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2016 The WebRTC Project Authors.
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

#ifndef _OCTK_RTC_STATS_REPORT_HPP
#define _OCTK_RTC_STATS_REPORT_HPP

#include <octk_scoped_refptr.hpp>
#include <octk_media_global.hpp>
#include <octk_rtc_stats.hpp>
#include <octk_timestamp.hpp>
#include <octk_ref_count.hpp>

#include <utility>
#include <string>
#include <vector>
#include <map>

OCTK_BEGIN_NAMESPACE

/**
 * @brief A collection of stats.
 * This is accessible as a map from `RtcStats::id` to `RtcStats`.
 */
class OCTK_MEDIA_API RtcStatsReport final : public RefCountedNonVirtual<RtcStatsReport>
{
public:
    using StatsMap = std::map<std::string, std::unique_ptr<const RtcStats>>;
    using SharedPtr = std::shared_ptr<RtcStatsReport>;
    using RefPtr = ScopedRefPtr<RtcStatsReport>;

    class OCTK_MEDIA_API ConstIterator
    {
    public:
        ConstIterator(ConstIterator &&other);
        ~ConstIterator();

        ConstIterator &operator++();
        ConstIterator &operator++(int);
        const RtcStats &operator*() const;
        const RtcStats *operator->() const;
        bool operator==(const ConstIterator &other) const;
        bool operator!=(const ConstIterator &other) const;

    private:
        friend class RtcStatsReport;
        ConstIterator(const ScopedRefPtr<const RtcStatsReport> &report, StatsMap::const_iterator iter);

        // Reference report to make sure it is kept alive.
        ScopedRefPtr<const RtcStatsReport> mReport;
        StatsMap::const_iterator mIter;
    };

    static ScopedRefPtr<RtcStatsReport> create(Timestamp timestamp);

    explicit RtcStatsReport(Timestamp timestamp);

    RtcStatsReport(const RtcStatsReport &other) = delete;
    ScopedRefPtr<RtcStatsReport> copy() const;

    Timestamp timestamp() const { return mTimestamp; }
    void addStats(std::unique_ptr<const RtcStats> stats);

    /**
     * @brief On success, returns a non-owning pointer to `stats`.
     * If the stats ID is not unique, `stats` is not inserted and nullptr is returned.
     * @tparam T
     * @param stats
     * @return
     */
    template <typename T> T *tryAddStats(std::unique_ptr<T> stats)
    {
        T *statsPtr = stats.get();
        if (!mStatsMap.insert(std::make_pair(std::string(stats->id()), std::move(stats))).second)
        {
            return nullptr;
        }
        return statsPtr;
    }
    const RtcStats *get(const std::string &id) const;
    size_t size() const { return mStatsMap.size(); }

    /**
     * @brief Gets the stat object of type `T` by ID, where `T` is any class descending from `RtcStats`.
     * Returns null if there is no stats object for the given ID or it is the wrong type.
     * @tparam T
     * @param id
     * @return
     */
    template <typename T> const T *getAs(const std::string &id) const
    {
        const RtcStats *stats = get(id);
        if (!stats || stats->type() != T::kType)
        {
            return nullptr;
        }
        return &stats->cast_to<const T>();
    }

    /**
     * @brief Removes the stats object from the report, returning ownership of it or null if there is no object with `id`.
     * @param id
     * @return
     */
    std::unique_ptr<const RtcStats> take(const std::string &id);

    /**
     * @brief Takes ownership of all the stats in `other`, leaving it empty.
     * @param other
     */
    void takeMembersFrom(ScopedRefPtr<RtcStatsReport> other);

    /**
     * @brief Stats iterators. Stats are ordered lexicographically on `RtcStats::id`.
     * @return
     */
    ConstIterator begin() const;
    ConstIterator end() const;

    /**
     * @brief Gets the subset of stats that are of type `T`, where `T` is any class descending from `RtcStats`.
     * @tparam T
     * @return
     */
    template <typename T> std::vector<const T *> getStatsOfType() const
    {
        std::vector<const T *> statsOfType;
        for (const RtcStats &stats : *this)
        {
            if (stats.type() == T::kType)
            {
                statsOfType.push_back(&stats.cast_to<const T>());
            }
        }
        return statsOfType;
    }

    /**
     * @brief Creates a JSON readable string representation of the report, listing all of its stats objects.
     * @return
     */
    std::string toJson() const;

protected:
    friend class RefCountedNonVirtual<RtcStatsReport>;
    ~RtcStatsReport() = default;

private:
    Timestamp mTimestamp;
    StatsMap mStatsMap;
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTC_STATS_REPORT_HPP
