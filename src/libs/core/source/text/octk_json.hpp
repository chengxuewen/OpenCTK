#ifndef _OCTK_JSON_HPP
#define _OCTK_JSON_HPP

#include <octk_global.hpp>
#include <octk_exception.hpp>

#include <nlohmann/json.hpp>

OCTK_BEGIN_NAMESPACE

using Json = nlohmann::json;

static inline Expected<Json, std::string> parseJson(const std::string &data)
{
    return tryCatchCall<Json>([data]() { return Json::parse(data); });
}

OCTK_END_NAMESPACE

#endif // _OCTK_JSON_HPP
