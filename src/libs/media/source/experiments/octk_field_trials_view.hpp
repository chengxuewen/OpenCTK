/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2019 The WebRTC project authors. All Rights Reserved.
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

#include <octk_string_view.hpp>
#include <octk_media_global.hpp>

#include <set>

OCTK_BEGIN_NAMESPACE

/**
 * @brief An interface that provides the means to access field trials.
 *
 * Note that there are no guarantess that the meaning of a particular key-value
 * mapping will be preserved over time and no announcements will be made if they
 * are changed. It's up to the library user to ensure that the behavior does not
 * break.
 */
class OCTK_MEDIA_API FieldTrialsView
{
public:
    template <typename T>
    using Set = std::set<T>;

    virtual ~FieldTrialsView() = default;

    bool IsEnabled(StringView key) const;

    bool IsDisabled(StringView key) const;

    /**
     * @brief Returns the configured value for `key` or an empty string if the field trial isn't configured.
     *
     * @param key
     * @return std::string
     */
    virtual std::string Lookup(StringView key) const = 0;
};

OCTK_END_NAMESPACE