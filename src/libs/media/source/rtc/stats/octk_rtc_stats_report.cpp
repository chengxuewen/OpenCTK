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

#include <octk_rtc_stats_report.hpp>

OCTK_BEGIN_NAMESPACE

RtcStatsReport::ConstIterator::ConstIterator(const SharedRefPtr<const RtcStatsReport> &report,
                                             StatsMap::const_iterator iter)
    : mReport(report)
    , mIter(iter)
{
}

RtcStatsReport::ConstIterator::ConstIterator(ConstIterator &&other) = default;

RtcStatsReport::ConstIterator::~ConstIterator() { }

RtcStatsReport::ConstIterator &RtcStatsReport::ConstIterator::operator++()
{
    ++mIter;
    return *this;
}

RtcStatsReport::ConstIterator &RtcStatsReport::ConstIterator::operator++(int) { return ++(*this); }

const RtcStats &RtcStatsReport::ConstIterator::operator*() const { return *mIter->second.get(); }

const RtcStats *RtcStatsReport::ConstIterator::operator->() const { return mIter->second.get(); }

bool RtcStatsReport::ConstIterator::operator==(const RtcStatsReport::ConstIterator &other) const
{
    return mIter == other.mIter;
}

bool RtcStatsReport::ConstIterator::operator!=(const RtcStatsReport::ConstIterator &other) const
{
    return !(*this == other);
}

SharedRefPtr<RtcStatsReport> RtcStatsReport::create(Timestamp timestamp)
{
    return SharedRefPtr<RtcStatsReport>(new RtcStatsReport(timestamp));
}

RtcStatsReport::RtcStatsReport(Timestamp timestamp)
    : mTimestamp(timestamp)
{
}

SharedRefPtr<RtcStatsReport> RtcStatsReport::copy() const
{
    SharedRefPtr<RtcStatsReport> copy = this->create(mTimestamp);
    for (auto iter = mStatsMap.begin(); iter != mStatsMap.end(); ++iter)
    {
        copy->addStats(iter->second->copy());
    }
    return copy;
}

void RtcStatsReport::addStats(std::unique_ptr<const RtcStats> stats)
{
#if OCTK_DCHECK_IS_ON
    auto result =
#endif
        mStatsMap.insert(std::make_pair(std::string(stats->id()), std::move(stats)));
#if OCTK_DCHECK_IS_ON
    OCTK_DCHECK(result.second) << "A stats object with ID \"" << result.first->second->id() << "\" is "
                               << "already present in this stats report.";
#endif
}

const RtcStats *RtcStatsReport::get(const std::string &id) const
{
    StatsMap::const_iterator iter = mStatsMap.find(id);
    if (mStatsMap.cend() != iter)
    {
        return iter->second.get();
    }
    return nullptr;
}

std::unique_ptr<const RtcStats> RtcStatsReport::take(const std::string &id)
{
    StatsMap::iterator iter = mStatsMap.find(id);
    if (mStatsMap.end() != iter)
    {
        std::unique_ptr<const RtcStats> stats = std::move(iter->second);
        mStatsMap.erase(iter);
        return stats;
    }
    return nullptr;
}

void RtcStatsReport::takeMembersFrom(SharedRefPtr<RtcStatsReport> other)
{
    for (StatsMap::iterator iter = other->mStatsMap.begin(); iter != other->mStatsMap.end(); ++iter)
    {
        this->addStats(std::unique_ptr<const RtcStats>(iter->second.release()));
    }
    other->mStatsMap.clear();
}

RtcStatsReport::ConstIterator RtcStatsReport::begin() const
{
    return ConstIterator(SharedRefPtr<const RtcStatsReport>(this), mStatsMap.cbegin());
}

RtcStatsReport::ConstIterator RtcStatsReport::end() const
{
    return ConstIterator(SharedRefPtr<const RtcStatsReport>(this), mStatsMap.cend());
}

std::string RtcStatsReport::toJson() const
{
    if (this->begin() == this->end())
    {
        return "";
    }
    std::stringstream ss;
    ss << "[";
    const char *separator = "";
    for (ConstIterator iter = this->begin(); iter != this->end(); ++iter)
    {
        ss << separator << iter->toJson();
        separator = ",";
    }
    ss << "]";
    return ss.str();
}

OCTK_END_NAMESPACE
