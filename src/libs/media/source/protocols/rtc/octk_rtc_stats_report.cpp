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

using RtcStatsMap = std::map<std::string, RtcStats::SharedPtr>;

class RtcStatsReport::ConstIteratorPrivate
{
public:
    explicit ConstIteratorPrivate(ConstIterator *p, const RtcStatsReport *report)
        : mPPtr(p)
        , mReport(report)
    {
    }
    virtual ~ConstIteratorPrivate() { }

    // Reference report to make sure it is kept alive.
    const RtcStatsReport *mReport{nullptr};
    RtcStatsMap::const_iterator mIter;

private:
    OCTK_DEFINE_PPTR(ConstIterator)
    OCTK_DECLARE_PUBLIC(ConstIterator)
    OCTK_DISABLE_COPY_MOVE(ConstIteratorPrivate)
};

RtcStatsReport::ConstIterator::ConstIterator(const RtcStatsReport *report)
    : mDPtr(new ConstIteratorPrivate(this, report))
{
}

RtcStatsReport::ConstIterator::~ConstIterator()
{
}

RtcStatsReport::ConstIterator &RtcStatsReport::ConstIterator::operator++()
{
    OCTK_D(ConstIterator);
    ++d->mIter;
    return *this;
}

RtcStatsReport::ConstIterator &RtcStatsReport::ConstIterator::operator++(int)
{
    return ++(*this);
}

const RtcStats &RtcStatsReport::ConstIterator::operator*() const
{
    OCTK_D(const ConstIterator);
    return *d->mIter->second.get();
}

const RtcStats *RtcStatsReport::ConstIterator::operator->() const
{
    OCTK_D(const ConstIterator);
    return d->mIter->second.get();
}

bool RtcStatsReport::ConstIterator::operator==(const RtcStatsReport::ConstIterator &other) const
{
    OCTK_D(const ConstIterator);
    return d->mIter == other.dFunc()->mIter;
}

bool RtcStatsReport::ConstIterator::operator!=(const RtcStatsReport::ConstIterator &other) const
{
    return !(*this == other);
}

class RtcStatsReportPrivate
{
public:
    RtcStatsReportPrivate(RtcStatsReport *p, Timestamp timestamp);
    virtual ~RtcStatsReportPrivate();

    Timestamp mTimestamp;
    RtcStatsMap mStatsMap;

private:
    OCTK_DEFINE_PPTR(RtcStatsReport)
    OCTK_DECLARE_PUBLIC(RtcStatsReport)
    OCTK_DISABLE_COPY_MOVE(RtcStatsReportPrivate)
};

RtcStatsReportPrivate::RtcStatsReportPrivate(RtcStatsReport *p, Timestamp timestamp)
    : mPPtr(p)
    , mTimestamp(timestamp)
{
}

RtcStatsReportPrivate::~RtcStatsReportPrivate()
{
}

RtcStatsReport::SharedPtr RtcStatsReport::create(Timestamp timestamp)
{
    return SharedPtr(new RtcStatsReport(timestamp), [](RtcStatsReport *p) { delete p; });
}

RtcStatsReport::RtcStatsReport(Timestamp timestamp)
    : mDPtr(new RtcStatsReportPrivate(this, timestamp))
{
}

RtcStatsReport::SharedPtr RtcStatsReport::copy() const
{
    OCTK_D(const RtcStatsReport);
    auto copy = this->create(d->mTimestamp);
    for (auto iter = d->mStatsMap.begin(); iter != d->mStatsMap.end(); ++iter)
    {
        copy->addStats(iter->second);
    }
    return copy;
}

void RtcStatsReport::addStats(const RtcStats::SharedPtr &stats)
{
    OCTK_D(RtcStatsReport);
    auto result = d->mStatsMap.insert(std::make_pair(stats->id().data(), stats));
#if OCTK_DCHECK_IS_ON
    OCTK_DCHECK(result.second) << "A stats object with ID \"" << result.first->second->id().data() << "\" is "
                               << "already present in this stats report.";
#else
    OCTK_UNUSED(result);
#endif
}

RtcStats::SharedPtr RtcStatsReport::get(StringView id) const
{
    OCTK_D(const RtcStatsReport);
    auto iter = d->mStatsMap.find(id.data());
    if (d->mStatsMap.cend() != iter)
    {
        return iter->second;
    }
    return nullptr;
}

RtcStats::SharedPtr RtcStatsReport::take(StringView id)
{
    OCTK_D(RtcStatsReport);
    auto iter = d->mStatsMap.find(id.data());
    if (d->mStatsMap.end() != iter)
    {
        auto stats = std::move(iter->second);
        d->mStatsMap.erase(iter);
        return stats;
    }
    return nullptr;
}

#if 0
void RtcStatsReport::takeMembersFrom(SharedRefPtr<RtcStatsReport> other)
{
    for (RtcStatsMap::iterator iter = other->mStatsMap.begin(); iter != other->mStatsMap.end(); ++iter)
    {
        this->addStats(std::unique_ptr<const RtcStats>(iter->second.release()));
    }
    other->mStatsMap.clear();
}

#endif

RtcStatsReport::ConstIterator RtcStatsReport::begin() const
{
    OCTK_D(const RtcStatsReport);
    ConstIterator iter(this);
    iter.dFunc()->mIter = d->mStatsMap.cbegin();
    return iter;
}

RtcStatsReport::ConstIterator RtcStatsReport::end() const
{
    OCTK_D(const RtcStatsReport);
    ConstIterator iter(this);
    iter.dFunc()->mIter = d->mStatsMap.cend();
    return iter;
}

String RtcStatsReport::toJson() const
{
    OCTK_D(const RtcStatsReport);
    if (this->begin() == this->end())
    {
        return "";
    }
    std::stringstream ss;
    ss << "[";
    const char *separator = "";
    for (auto iter = this->begin(); iter != this->end(); ++iter)
    {
        ss << separator << iter->toJson().data();
        separator = ",";
    }
    ss << "]";
    return ss.str();
}

OCTK_END_NAMESPACE
