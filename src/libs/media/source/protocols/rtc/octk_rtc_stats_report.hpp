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

#pragma once

#include <octk_media_global.hpp>
#include <octk_rtc_stats.hpp>
#include <octk_timestamp.hpp>
#include <octk_memory.hpp>

OCTK_BEGIN_NAMESPACE

/**
 * @brief A collection of stats.
 * This is accessible as a map from `RtcStats::id` to `RtcStats`.
 */
class RtcStatsReportPrivate;
class OCTK_MEDIA_API RtcStatsReport final
{
public:
    using SharedPtr = SharedPointer<RtcStatsReport>;

    class ConstIteratorPrivate;
    class OCTK_MEDIA_API ConstIterator final
    {
    public:
        ConstIterator(ConstIterator &&other)
            : ConstIterator()
        {
            std::swap(mDPtr, other.mDPtr);
        }
        ~ConstIterator();

        ConstIterator &operator++();
        ConstIterator &operator++(int);
        const RtcStats &operator*() const;
        const RtcStats *operator->() const;
        bool operator==(const ConstIterator &other) const;
        bool operator!=(const ConstIterator &other) const;

    protected:
        friend class RtcStatsReport;
        ConstIterator(const RtcStatsReport *report = nullptr);

    private:
        OCTK_DEFINE_DPTR(ConstIterator)
        OCTK_DECLARE_PRIVATE(ConstIterator)
        OCTK_DECLARE_DISABLE_COPY(ConstIterator)
    };

    static SharedPtr create(Timestamp timestamp);

    explicit RtcStatsReport(Timestamp timestamp);

    SharedPtr copy() const;

    size_t size() const;
    Timestamp timestamp() const;
    RtcStats::SharedPtr get(StringView id) const;

    void addStats(const RtcStats::SharedPtr &stats);

#if 0
    /**
     * @brief On success, returns a non-owning pointer to `stats`.
     * If the stats ID is not unique, `stats` is not inserted and nullptr is returned.
     * @tparam T
     * @param stats
     * @return
     */
    template <typename T>
    T *tryAddStats(const SharedPointer<T> stats)
    {
        T *statsPtr = stats.get();
        if (!mStatsMap.insert(std::make_pair(std::string(stats->id()), std::move(stats))).second)
        {
            return nullptr;
        }
        return statsPtr;
    }

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
#endif

    /**
     * @brief Removes the stats object from the report, returning ownership of it or null if there is no object with `id`.
     * @param id
     * @return
     */
    RtcStats::SharedPtr take(StringView id);

#if 0
    /**
     * @brief Takes ownership of all the stats in `other`, leaving it empty.
     * @param other
     */
    void takeMembersFrom(SharedRefPtr<RtcStatsReport> other);

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
#endif

    /**
   * @brief Stats iterators. Stats are ordered lexicographically on `RtcStats::id`.
   * @return
   */
    ConstIterator begin() const;
    ConstIterator end() const;

    /**
     * @brief Creates a JSON readable string representation of the report, listing all of its stats objects.
     * @return
     */
    String toJson() const;

protected:
    ~RtcStatsReport() = default;

private:
    OCTK_DEFINE_DPTR(RtcStatsReport)
    OCTK_DECLARE_PRIVATE(RtcStatsReport)
    OCTK_DISABLE_COPY_MOVE(RtcStatsReport)
};

OCTK_END_NAMESPACE