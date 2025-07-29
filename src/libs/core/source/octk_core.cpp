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

#include <octk_core.hpp>
#include <octk_logging.hpp>
#include <octk_core_config.hpp>

#include <mutex>

OCTK_BEGIN_NAMESPACE
namespace detail
{
static std::once_flag initOnceFlag;
}

Core::Core()
{
}

void Core::init()
{
    std::call_once(detail::initOnceFlag, [=]()
    {
    });
}

const char *Core::version()
{
    return OCTK_VERSION_NAME;
}

OCTK_END_NAMESPACE

#include <octk_core.h>

void octk_logger_set_level_enable(int id, octk_log_level_t level, bool enable)
{
    auto logger = octk::Logger::logger(id);
    if (logger)
    {
        logger->setLevelEnable(static_cast<octk::LogLevel>(level), enable);
    }
}

bool octk_logger_is_level_enabled(int id, octk_log_level_t level)
{
    auto logger = octk::Logger::logger(id);
    if (logger)
    {
        logger->isLevelEnabled(static_cast<octk::LogLevel>(level));
    }
    return false;
}

void octk_logger_switch_level(int id, octk_log_level_t level)
{
    auto logger = octk::Logger::logger(id);
    if (logger)
    {
        logger->switchLevel(static_cast<octk::LogLevel>(level));
    }
}

static void init_logger(octk::Logger::Ptr logger,
                        octk_log_level_t level,
                        octk_log_callback_func func,
                        bool unique_ownership)
{
    if (logger)
    {
        octk::Logger::MessageHandler handler = nullptr;
        if (func)
        {
            handler = [func](const char *name, const octk::Logger::Context &context, const char *message)
            {
                func(name,
                     {
                         static_cast<octk_log_level_t>(context.level),
                         context.filePath,
                         context.fileName,
                         context.funcName,
                         context.line
                     },
                     message);
            };
        }
        logger->installMessageHandler(handler, unique_ownership);
        logger->switchLevel(static_cast<octk::LogLevel>(level));
    }
}

void octk_init_all_loggers(octk_log_level_t level, octk_log_callback_func func, bool unique_ownership)
{
    auto loggers = octk::Logger::allLoggers();
    for (auto &&logger : loggers)
    {
        init_logger(logger, level, func, unique_ownership);
    }
}

void octk_init_logger(int id, octk_log_level_t level, octk_log_callback_func func, bool unique_ownership)
{
    auto logger = octk::Logger::logger(id);
    init_logger(logger, level, func, unique_ownership);
}

int octk_logger_id(const char *name)
{
    return octk::Logger::loggerIdNumber(name);
}

const char *octk_core_version()
{
    return octk::Core::version();
}

void octk_core_init()
{
    octk::Core::init();
}
//
// void octk_utc_timestamp_string(char *buff, size_t len)
// {
//     if (buff && len > 0)
//     {
//         const auto timestamp = octk::time_utils::utcTimestampString();
//         snprintf(buff, len, "%s", timestamp.c_str());
//     }
// }
//
// void octk_local_timestamp_string(char *buff, size_t len)
// {
//     if (buff && len > 0)
//     {
//         const auto timestamp = octk::time_utils::localTimestampString();
//         snprintf(buff, len, "%s", timestamp.c_str());
//     }
// }
