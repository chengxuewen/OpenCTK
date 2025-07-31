/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
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

#ifndef _OCTK_LOGGING_HPP
#define _OCTK_LOGGING_HPP

#include <octk_global.hpp>
#include <octk_string_utils.hpp>

#include <vector>
#include <string>
#include <ostream>
#include <sstream>
#include <functional>

OCTK_BEGIN_NAMESPACE

enum class LogLevel : int
{
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6,
};

static constexpr int LogLevelNum = 7;

class LoggerPrivate;

class OCTK_CORE_API Logger
{
public:
    using Ptr = Logger *;

    struct Context final
    {
        LogLevel level;
        const char *filePath;
        const char *fileName;
        const char *funcName;
        int line;
    };

    class Stream final
    {
        struct StreamData
        {
            StreamData(Logger &target, const Context &ctx, bool spa)
                : ref(1), space(spa), logger(target), context(ctx)
            {
            }

            int ref;
            bool space;
            Logger &logger;
            std::stringstream ss;
            const Context &context;
        } *mStream;

    public:
        inline Stream(const Stream &other)
            : mStream(other.mStream) { ++mStream->ref; }

        inline Stream(Logger &target, const Context &ctx, bool spa = false)
            : mStream(new StreamData(target, ctx, spa))
        {
        }

        virtual ~Stream()
        {
            if (!--mStream->ref)
            {
                std::string buffer(mStream->ss.str());
                if (mStream->space && buffer.back() == ' ')
                {
                    buffer.pop_back();
                }
                mStream->logger.output(mStream->context, buffer.c_str());
                delete mStream;
            }
        }

        inline Stream &operator=(const Stream &other)
        {
            if (this != &other)
            {
                Stream copy(other);
                std::swap(mStream, copy.mStream);
            }
            return *this;
        }

        void putUcs2(uint16_t ucs2)
        {
        }

        void putUcs4(uint32_t ucs4)
        {
        }

        void putString(const char *begin, size_t length)
        {
        }

        OCTK_FORCE_INLINE void prepend(const char *message)
        {
            std::stringstream ss;
            ss << message << mStream->ss.get();
            std::swap(mStream->ss, ss);
        }

        inline Stream &space()
        {
            mStream->space = true;
            mStream->ss << ' ';
            return *this;
        }

        inline Stream &nospace()
        {
            mStream->space = false;
            return *this;
        }

        inline Stream &maybeSpace()
        {
            if (mStream->space)
            {
                mStream->ss << ' ';
            }
            return *this;
        }

        inline Stream &operator<<(bool t)
        {
            mStream->ss << (t ? "true" : "false");
            return this->maybeSpace();
        }

        inline Stream &operator<<(char t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(signed short t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(unsigned short t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(char16_t t)
        {
            this->putUcs2(t);
            return this->maybeSpace();
        }

        inline Stream &operator<<(char32_t t)
        {
            this->putUcs4(t);
            return this->maybeSpace();
        }

        inline Stream &operator<<(signed int t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(unsigned int t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(signed long t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(signed long long t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(unsigned long t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(unsigned long long t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(float t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(double t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(const char *t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(const std::string &t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(const void *t)
        {
            mStream->ss << t;
            return this->maybeSpace();
        }

        inline Stream &operator<<(std::nullptr_t)
        {
            mStream->ss << "(nullptr)";
            return this->maybeSpace();
        }

        inline Stream &operator<<(const std::stringstream &ss)
        {
            mStream->ss << ss.str();
            return this->maybeSpace();
        }

        inline Stream &operator<<(const StringView &t)
        {
            mStream->ss << t.data();
            return this->maybeSpace();
        }
    };

    struct Streamer final
    {
        Streamer(Logger &target, LogLevel level, const char *filePath, const char *funcName, int line)
            : logger(target), context{level, filePath, utils::extractFileName(filePath), funcName, line}
        {
        }

        void logging(const char *format, ...) const
        {
            va_list args;
            va_start(args, format);
            logger.vlogging(context, format, args);
            va_end(args);
        }

        OCTK_FORCE_INLINE Stream logging(const StringView &string) const
        {
            return Stream(logger, context, true) << string;
        }

        OCTK_FORCE_INLINE Stream logging() const
        {
            return Stream(logger, context);
        }

        Logger &logger;
        Context context;
    };

    class FatalLogCall final
    {
    public:
        FatalLogCall(const char *message) : mMessage(message)
        {
        }

        OCTK_FORCE_INLINE void operator&(Stream stream)
        {
            stream.prepend(mMessage);
        }

    private:
        const char *mMessage;
    };

    Logger(const char *name, LogLevel defaultLevel = LogLevel::Debug);
    explicit Logger(LoggerPrivate *d);
    virtual ~Logger();

    static Logger::Ptr logger(int idNumber);
    static Logger::Ptr logger(const char *name);
    static int loggerIdNumber(const char *name);
    static const char *loggerName(int idNumber);
    static std::vector<Logger::Ptr> allLoggers();

    int idNumber() const;
    const char *name() const;

    bool isNoSource() const;
    void setNoSource(bool noSource);

    void switchLevel(LogLevel level);
    bool isLevelEnabled(LogLevel level) const;
    void setLevelEnable(LogLevel level, bool enable);

    void output(const Context &context, const char *message);
    void vlogging(const Context &context, const char *format, va_list args);

    using MessageHandler = std::function<void(const char *name, const Context &context, const char *message)>;
    void installMessageHandler(const MessageHandler &handler, bool uniqueOwnership = false);

protected:
    void fatalAbort();

    OCTK_DEFINE_DPTR(Logger)
    OCTK_DECLARE_PRIVATE(Logger)
    OCTK_DISABLE_COPY_MOVE(Logger)
};

OCTK_END_NAMESPACE

#define OCTK_DECLARE_LOGGER(export, logger) export octk::Logger &logger();
#define OCTK_DEFINE_LOGGER_WITH_LEVEL(name, logger, level) \
    octk::Logger &logger() { \
    static constexpr char loggerName[] = name; \
    static octk::Logger loggerInstance(loggerName, level);  \
    return loggerInstance; } \
    static struct logger##_builder{ logger##_builder() { logger(); } } init_##logger;

#define OCTK_DEFINE_LOGGER(name, logger) OCTK_DEFINE_LOGGER_WITH_LEVEL(name, logger, octk::LogLevel::Debug)

#define OCTK_LOGGING(logger, level, ...) \
    for (bool enabled = logger.isLevelEnabled(level); enabled; enabled = false) \
    octk::Logger::Streamer(logger, level, __FILE__, OCTK_STRFUNC, __LINE__).logging(__VA_ARGS__);

#define OCTK_LOGGING_TRACE(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Trace, __VA_ARGS__)
#define OCTK_LOGGING_DEBUG(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Debug, __VA_ARGS__)
#define OCTK_LOGGING_INFO(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Info, __VA_ARGS__)
#define OCTK_LOGGING_WARNING(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Warning, __VA_ARGS__)
#define OCTK_LOGGING_ERROR(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Error, __VA_ARGS__)
#define OCTK_LOGGING_CRITICAL(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Critical, __VA_ARGS__)
#define OCTK_LOGGING_FATAL(logger, ...) OCTK_LOGGING(logger, octk::LogLevel::Fatal, __VA_ARGS__)

OCTK_DECLARE_LOGGER(OCTK_CORE_API, OCTK_LOGGER)
#define OCTK_TRACE \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Trace); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Trace, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_DEBUG \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Debug); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Debug, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_INFO \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Info); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Info, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_WARNING \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Warning); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Warning, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_ERROR \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Error); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Error, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_CRITICAL \
    for (bool enabled = OCTK_LOGGER().isLevelEnabled(octk::LogLevel::Critical); enabled; enabled = false) \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Critical, __FILE__, OCTK_STRFUNC, __LINE__).logging
#define OCTK_FATAL \
    octk::Logger::Streamer(OCTK_LOGGER(), octk::LogLevel::Fatal, __FILE__, OCTK_STRFUNC, __LINE__).logging

#endif // _OCTK_LOGGING_HPP
