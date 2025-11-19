#include <iostream>
#include <utility>

#include <octk_logging.hpp>
#include <octk_variant.hpp>
#include <octk_string_utils.hpp>

OCTK_DEFINE_LOGGER("my_log", MY_LOGGER)

namespace expns
{
std::pair<int, double> myfunction(int a, double b)
{
    OCTK_WARNING("funcname:%s, extract:%s", OCTK_STRFUNC, OCTK_STRFUNC_NAME);
    return std::make_pair(a, b);
}
} // namespace expns

int main()
{
    std::cout << "octk_logging exp!\n" << std::endl;
    expns::myfunction(1, 2);
    return 0;
    std::cout << "\noctk_logging c!" << std::endl;

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
