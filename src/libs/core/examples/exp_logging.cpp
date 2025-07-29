#include <iostream>

#include <octk_core.h>
#include <octk_logging.hpp>
#include <octk_variant.hpp>

OCTK_DEFINE_LOGGER("my_log", MY_LOGGER)


void message_handler(const char *name,  octk_log_context_t context, const char *message)
{
    std::cout << "logger=" << name 
    << ", context.level=" << context.level
    << ", context.filePath=" << context.filePath
    << ", context.fileName=" << context.fileName
    << ", context.funcName=" << context.funcName
    << ", context.line=" << context.line
    << std::endl;
}

int main()
{
    std::cout << "octk_logging exp!\n" << std::endl;

    std::cout << "\noctk_logging c!" << std::endl;
    const auto loggerId = octk_logger_id("octk");
    octk_init_logger(loggerId, OCTK_LOG_LEVEL_TRACE, message_handler, true);
    OCTK_TRACE("OCTK_TRACE");
    OCTK_DEBUG("OCTK_DEBUG");
    OCTK_INFO("OCTK_INFO");
    OCTK_WARNING("OCTK_WARN");
    OCTK_ERROR("OCTK_ERROR");
    OCTK_CRITICAL("OCTK_CRITICAL");
    octk_init_logger(loggerId, OCTK_LOG_LEVEL_TRACE, nullptr, false);

    std::cout << "\noctk_logging cxx!" << std::endl;
    OCTK_LOGGING_WARNING(MY_LOGGER(), "OCTK_LOGGING_WARN");

    OCTK_TRACE("OCTK_TRACE");
    OCTK_DEBUG("OCTK_DEBUG"); 
    OCTK_INFO("OCTK_INFO"); 
    OCTK_WARNING("OCTK_WARN"); 
    OCTK_ERROR("OCTK_ERROR"); 
    OCTK_CRITICAL("OCTK_CRITICAL"); 
    // OCTK_FATAL("OCTK_FATAL"); 
    
    OCTK_WARNING() << octk::StringView("OCTK_WARNING StringView");

    std::cout << "log all!" << std::endl;
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);
    OCTK_TRACE("OCTK_TRACE");
    OCTK_TRACE() << "stream OCTK_TRACE";
    OCTK_DEBUG("OCTK_DEBUG");
    OCTK_DEBUG() << "stream OCTK_DEBUG";
    OCTK_INFO("OCTK_INFO");
    OCTK_INFO() << "stream OCTK_INFO";
    OCTK_WARNING("OCTK_WARN");
    OCTK_WARNING() << "stream OCTK_WARN";
    OCTK_ERROR("OCTK_ERROR"); 
    OCTK_ERROR() << "stream OCTK_ERROR";
    OCTK_CRITICAL("OCTK_CRITICAL");
    OCTK_CRITICAL() << "stream OCTK_CRITICAL";
    OCTK_FATAL("OCTK_FATAL");
    OCTK_FATAL() << "stream OCTK_FATAL";
    
    return 0;
}
