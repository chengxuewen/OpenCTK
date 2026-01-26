/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2022 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_global.hpp>
#include <octk_field_trials_view.hpp>

OCTK_BEGIN_NAMESPACE

// Abstract base class for a field trial registry that verifies that any looked
// up key has been pre-registered in accordance with `g3doc/field-trials.md`.
class OCTK_MEDIA_API FieldTrialsRegistry : public FieldTrialsView
{
public:
    FieldTrialsRegistry() = default;

    FieldTrialsRegistry(const FieldTrialsRegistry &) = default;
    FieldTrialsRegistry &operator=(const FieldTrialsRegistry &) = default;

    ~FieldTrialsRegistry() override = default;

    // Verifies that `key` is a registered field trial and then returns the
    // configured value for `key` or an empty string if the field trial isn't
    // configured.
    std::string Lookup(StringView key) const override;

    // Register additional `keys` for testing. This should only be used for
    // imaginary keys that are never used outside test code.
    void RegisterKeysForTesting(Set<std::string> keys) { test_keys_ = std::move(keys); }

private:
    virtual std::string GetValue(StringView key) const = 0;

    // Imaginary keys only used for testing.
    Set<std::string> test_keys_;
};

OCTK_END_NAMESPACE