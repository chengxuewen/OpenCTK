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

#include <private/octk_logging_p.hpp>
#include <octk_memory.hpp>
#include <octk_assert.hpp>

#include <unordered_map>
#ifndef OCTK_OS_WIN32
#    include <stdlib.h> // abort
#else
#    include <windows.h>
#endif

OCTK_DEFINE_LOGGER("octk", OCTK_LOGGER)

OCTK_BEGIN_NAMESPACE

namespace detail
{
static std::mutex &loggersMapMutex()
{
    static std::mutex mutex;
    return mutex;
}

static std::unordered_map<int, Logger::Ptr> &loggersIdMap()
{
    static std::unordered_map<int, Logger::Ptr> map;
    return map;
}

static std::unordered_map<std::string, Logger::Ptr> &loggersNameMap()
{
    static std::unordered_map<std::string, Logger::Ptr> map;
    return map;
}

static std::atomic<int> &loggerIdNumberCounter()
{
    static std::atomic<int> counter = ATOMIC_VAR_INIT(0);
    return counter;
}
}; // namespace detail

LoggerPrivate::LoggerPrivate(Logger *p, const char *name)
    : mPPtr(p)
    , mIdNumber(detail::loggerIdNumberCounter().fetch_add(1))
    , mName(name)
    , mNoSource(false)
{
    std::lock_guard<std::mutex> locker(detail::loggersMapMutex());
    detail::loggersIdMap().emplace(mIdNumber, p);
    detail::loggersNameMap().emplace(name, p);
}

LoggerPrivate::~LoggerPrivate() { }

bool LoggerPrivate::messageHandlerOutput(const LoggerPrivate::Context &context, const char *message)
{
    const auto handlerWraper = mMessageHandlerWraper.load();
    if (handlerWraper)
    {
        handlerWraper->handler(mName, context, message);
        return mMessageHandleUniqueOwnership.load();
    }
    return false;
}

Logger::Logger(const char *name, LogLevel defaultLevel)
    : mDPtr(utils::makeUnique<LoggerPrivate>(this, name))
{
    std::vector<spdlog::sink_ptr> sinks;
    const auto baseFilename = "log/" + std::string(name) + "_daily";
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(baseFilename, 0, 0, false, 7));
    mDPtr->mLogger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    mDPtr->mLogger->set_level(spdlog::level::trace);
    mDPtr->mLogger->flush_on(spdlog::level::debug);
    this->switchLevel(defaultLevel);
}

Logger::Logger(LoggerPrivate *d)
    : mDPtr(d)
{
}

Logger::~Logger() { }

Logger::Ptr Logger::logger(int idNumber)
{
    std::lock_guard<std::mutex> locker(detail::loggersMapMutex());
    const auto iter = detail::loggersIdMap().find(idNumber);
    return detail::loggersIdMap().end() != iter ? iter->second : nullptr;
}

Logger::Ptr Logger::logger(const char *name)
{
    std::lock_guard<std::mutex> locker(detail::loggersMapMutex());
    const auto iter = detail::loggersNameMap().find(name);
    return detail::loggersNameMap().end() != iter ? iter->second : nullptr;
}

int Logger::loggerIdNumber(const char *name)
{
    auto logger = Logger::logger(name);
    return logger ? logger->idNumber() : -1;
}

const char *Logger::loggerName(int idNumber)
{
    auto logger = Logger::logger(idNumber);
    return logger ? logger->name() : nullptr;
}

std::vector<Logger::Ptr> Logger::allLoggers()
{
    std::vector<Logger::Ptr> loggers;
    std::lock_guard<std::mutex> locker(detail::loggersMapMutex());
    std::transform(detail::loggersNameMap().begin(),
                   detail::loggersNameMap().end(),
                   std::back_inserter(loggers),
                   [](const std::pair<std::string, Logger::Ptr> &pair) { return pair.second; });
    return loggers;
}

int Logger::idNumber() const
{
    OCTK_D(const Logger);
    return d->mIdNumber;
}

const char *Logger::name() const
{
    OCTK_D(const Logger);
    return d->mName;
}

bool Logger::isNoSource() const
{
    OCTK_D(const Logger);
    return d->mNoSource;
}

void Logger::setNoSource(bool noSource)
{
    OCTK_D(Logger);
    d->mNoSource = noSource;
}

void Logger::switchLevel(LogLevel level)
{
    OCTK_D(Logger);
    for (size_t i = 0; i < LogLevelNum; i++)
    {
        d->mLevelEnabled[i].store(i >= (int)level);
    }
}

bool Logger::isLevelEnabled(LogLevel level) const
{
    OCTK_D(const Logger);
    return d->mLevelEnabled[(int)level].load();
}

void Logger::setLevelEnable(LogLevel level, bool enable)
{
    OCTK_D(Logger);
    d->mLevelEnabled[(int)level].store(enable);
}

void Logger::output(const Context &context, const char *message)
{
    OCTK_D(Logger);
    if (!d->messageHandlerOutput(context, message))
    {
        if (d->mNoSource)
        {
            d->mLogger->log(static_cast<spdlog::level::level_enum>(context.level), message);
        }
        else
        {
            d->mLogger->log(spdlog::source_loc{context.filePath, context.line, context.funcName},
                            static_cast<spdlog::level::level_enum>(context.level),
                            message);
        }
    }
    if (octk::LogLevel::Fatal == context.level)
    {
        this->fatalAbort();
    }
}

void Logger::vlogging(const Context &context, const char *format, va_list args)
{
    OCTK_D(Logger);
    char message[OCTK_LOGGING_BUFFER_SIZE_MAX] = {0};
    vsnprintf(message, OCTK_LOGGING_BUFFER_SIZE_MAX, format, args);
    if (!d->messageHandlerOutput(context, message))
    {
        if (d->mNoSource)
        {
            d->mLogger->log(static_cast<spdlog::level::level_enum>(context.level), message);
        }
        else
        {
            d->mLogger->log(spdlog::source_loc{context.filePath, context.line, context.funcName},
                            static_cast<spdlog::level::level_enum>(context.level),
                            message);
        }
    }
    if (octk::LogLevel::Fatal == context.level)
    {
        this->fatalAbort();
    }
}

void Logger::installMessageHandler(const MessageHandler &handler, bool uniqueOwnership)
{
    OCTK_D(Logger);
    auto newWraper = handler ? new LoggerPrivate::MessageHandlerWraper(handler) : nullptr;
    auto oldWraper = d->mMessageHandlerWraper.exchange(newWraper);
    d->mMessageHandleUniqueOwnership.store(uniqueOwnership);
    if (oldWraper)
    {
        delete oldWraper;
    }
}

void Logger::fatalAbort()
{
#ifdef OCTK_OS_WIN32
    DebugBreak();
#endif
    abort();
}

OCTK_END_NAMESPACE

OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Trace) == SPDLOG_LEVEL_TRACE);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Debug) == SPDLOG_LEVEL_DEBUG);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Info) == SPDLOG_LEVEL_INFO);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Warning) == SPDLOG_LEVEL_WARN);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Error) == SPDLOG_LEVEL_ERROR);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Critical) == SPDLOG_LEVEL_CRITICAL);
OCTK_STATIC_ASSERT(static_cast<int>(octk::LogLevel::Fatal) == SPDLOG_LEVEL_OFF);
