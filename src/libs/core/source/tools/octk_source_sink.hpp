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

#include <octk_unique_function.hpp>
#include <octk_shared_pointer.hpp>

#include <mutex>
#include <set>

OCTK_BEGIN_NAMESPACE

template <typename Data>
class Sink
{
public:
    using DataType = Data;
    using SharedPtr = SharedPointer<Sink<DataType>>;

    virtual void onData(const Data &data) = 0;

protected:
    virtual ~Sink() = default;
};

template <typename Data>
class SinkCallback : public Sink<Data>
{
public:
    using DataType = Data;
    using SharedPtr = SharedPointer<Sink<DataType>>;
    using Callback = UniqueFunction<void(const Data &data)>;

    SinkCallback() = default;
    explicit SinkCallback(Callback callback) { this->setCallback(std::move(callback)); }
    ~SinkCallback() override = default;

    void setCallback(Callback callback) { mCallback = std::move(callback); }

    void onData(const Data &data) override
    {
        if (mCallback)
        {
            mCallback(data);
        }
    }

private:
    Callback mCallback;
};


template <typename Data>
class Source
{
public:
    using DataType = Data;
    using SinkType = Sink<DataType>;
    using SharedPtr = SharedPointer<Source<DataType>>;

    void addSink(SinkType *sink)
    {
        this->addSink(SharedPointer<SinkType>(sink, [](SinkType *) { }));
    }
    void removeSink(SinkType *sink)
    {
        this->removeSink(SharedPointer<SinkType>(sink, [](SinkType *) { }));
    }

    virtual std::set<SharedPointer<SinkType>> sinks() const = 0;
    virtual void addSink(const SharedPointer<SinkType> &sink) = 0;
    virtual void removeSink(const SharedPointer<SinkType> &sink) = 0;

protected:
    virtual ~Source() = default;
};

template <typename Data>
class SourceProvider : public Source<Data>
{
public:
    using BaseType = Source<Data>;
    using DataType = typename BaseType::DataType;
    using SinkType = typename BaseType::SinkType;
    using SharedPtr = SharedPointer<SourceProvider<DataType>>;

    SourceProvider() = default;
    ~SourceProvider() override = default;


    std::set<SharedPointer<SinkType>> sinks() const override { return mSinks; }
    void addSink(const SharedPointer<SinkType> &sink) override
    {
        const auto iter = std::find_if(mSinks.begin(),
                                       mSinks.end(),
                                       [sink](const SharedPointer<SinkType> &item)
                                       { return item.get() == sink.get(); });
        if (mSinks.end() == iter)
        {
            mSinks.insert(sink);
        }
    }
    void removeSink(const SharedPointer<SinkType> &sink) override
    {
        const auto iter = std::find_if(mSinks.begin(),
                                       mSinks.end(),
                                       [sink](const SharedPointer<SinkType> &item)
                                       { return item.get() == sink.get(); });
        if (mSinks.end() != iter)
        {
            mSinks.erase(iter);
        }
    }

protected:
    std::set<SharedPointer<SinkType>> mSinks;
};

template <typename Data>
class SourceBroadcaster : public SourceProvider<Data>, public Sink<Data>
{
public:
    using DataType = Data;
    using SinkType = Sink<DataType>;
    using SourceType = SourceProvider<DataType>;
    using SharedPtr = SharedPointer<SourceBroadcaster<DataType>>;

    SourceBroadcaster() = default;
    ~SourceBroadcaster() override = default;

    std::set<SharedPointer<SinkType>> sinks() const override
    {
        std::lock_guard<std::mutex> lock(mSinksMutex);
        return SourceType::sinks();
    }
    void addSink(const SharedPointer<SinkType> &sink) override
    {
        std::lock_guard<std::mutex> lock(mSinksMutex);
        SourceType::addSink(sink);
    }
    void removeSink(const SharedPointer<SinkType> &sink) override
    {
        std::lock_guard<std::mutex> lock(mSinksMutex);
        SourceType::removeSink(sink);
    }

    void pushData(const Data &data) { this->onData(data); }

protected:
    void onData(const Data &data) override
    {
        for (const auto &sink : this->sinks())
        {
            sink->onData(data);
        }
    }

    mutable std::mutex mSinksMutex;
};

OCTK_END_NAMESPACE
