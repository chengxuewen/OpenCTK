#include "worker.h"

#include <private/octk_webrtc_logger_p.hpp>

Worker::Worker(std::string name, std::function<void()> executing_function)
    : abort_(false)
    , name_(name)
    , executing_function_(executing_function)
{
}

Worker::~Worker()
{
    abort_.store(true);
    thread_.Finalize();
    OCTK_LOGGING_ERROR(WEBRTC_LOGGER(), "'%s' was released!", name_.c_str());
}

void Worker::Run()
{
    thread_ = rtc::PlatformThread::SpawnJoinable([this]() { this->Thread(); },
                                                 name_,
                                                 rtc::ThreadAttributes().SetPriority(rtc::ThreadPriority::kHigh));
}

void Worker::Thread()
{
    while (!abort_.load())
    {
        executing_function_();
    }
}
