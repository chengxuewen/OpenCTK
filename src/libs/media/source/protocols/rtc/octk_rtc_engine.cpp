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

#include <octk_rtc_engine.hpp>
#include <octk_logging.hpp>

OCTK_BEGIN_NAMESPACE

namespace detail
{
using RtcEngineCreaterMap = std::map<std::string, RtcEngine::Creater>;
using RtcEngineCreaterMapItem = std::pair<std::string, RtcEngine::Creater>;
static RtcEngineCreaterMap *rtcEngineCreaterMap()
{
    static RtcEngineCreaterMap map;
    return &map;
}
} // namespace detail

SharedPointer<RtcPeerConnectionFactory> RtcEngine::create(StringView backendName)
{
    auto map = detail::rtcEngineCreaterMap();
    auto iter = map->end();
    if (backendName.empty() && !map->empty())
    {
        iter = map->begin(); // use first default backend
    }
    else
    {
        iter = map->find(backendName.data());
    }
    if (map->end() != iter)
    {
        if (iter->second.createFunc)
        {
            return iter->second.createFunc();
        }
    }
    return nullptr;
}

void RtcEngine::registerFactory(StringView backendName, Creater creater)
{
    auto map = detail::rtcEngineCreaterMap();
    const auto iter = map->find(backendName.data());
    if (map->end() != iter)
    {
        OCTK_FATAL("RtcEngine::registerFactory: backendName %s already registered.", backendName.data());
    }
    if (creater.initializeFunc)
    {
        creater.initializeFunc();
    }
    map->insert(std::make_pair(backendName, creater));
}

std::vector<std::string> RtcEngine::registeredTypes()
{
    std::vector<std::string> keys;
    auto map = detail::rtcEngineCreaterMap();
    keys.reserve(map->size());
    std::transform(map->begin(),
                   map->end(),
                   std::back_inserter(keys),
                   [](const detail::RtcEngineCreaterMapItem &pair) { return pair.first; });
    return keys;
}

void RtcEngine::switchLogLevel(LogLevel level, StringView backendName)
{
    auto map = detail::rtcEngineCreaterMap();
    auto iter = map->end();
    if (backendName.empty() && !map->empty())
    {
        iter = map->begin(); // use first default backend
    }
    else
    {
        iter = map->find(backendName.data());
    }
    if (map->end() != iter)
    {
        if (iter->second.switchLogLevelFunc)
        {
            iter->second.switchLogLevelFunc(level);
        }
    }
    else
    {
        for (const auto &item : *map)
        {
            if (item.second.switchLogLevelFunc)
            {
                item.second.switchLogLevelFunc(level);
            }
        }
    }
}

OCTK_END_NAMESPACE
