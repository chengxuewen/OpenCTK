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

#pragma once

#include <octk_exception.hpp>

#include <nlohmann/json.hpp>

OCTK_BEGIN_NAMESPACE

using Json = nlohmann::json;

namespace utils
{
static OCTK_FORCE_INLINE Expected<Json, std::string> parseJson(const std::string &data)
{
    return tryCatchCall<Json>([data]() { return Json::parse(data); });
}

template <typename T> static OCTK_FORCE_INLINE bool parseJsonToVector(const Json &json, std::vector<T> *out = nullptr)
{
    if (json.is_array())
    {
        if (out)
        {
            out->clear();
            for (const auto &item : json)
            {
                auto expected = tryCatchCall<T>([&]() { return item.get<T>(); });
                if (expected.has_value())
                {
                    out->push_back(expected.value());
                }
            }
            return true;
        }
    }
    return false;
}

template <typename T> static OCTK_FORCE_INLINE bool readJsonValue(const Json &json, StringView key, T *out = nullptr)
{
    const auto iter = json.find(key.data());
    if (json.end() != iter)
    {
        if (out)
        {
            *out = iter->get<T>();
        }
        return true;
    }
    return false;
}
} // namespace utils

OCTK_END_NAMESPACE
