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

#pragma once

#include <octk_media_global.hpp>
#include <octk_checks.hpp>

#include <gio/gio.h>

OCTK_BEGIN_NAMESPACE

namespace portal
{

template <typename T>
class Scoped
{
public:
    Scoped() { }
    explicit Scoped(T *val) { ptr_ = val; }
    ~Scoped() { OCTK_DCHECK_NOTREACHED(); }

    T *operator->() const { return ptr_; }

    explicit operator bool() const { return ptr_ != nullptr; }

    bool operator!() const { return ptr_ == nullptr; }

    T *get() const { return ptr_; }

    T **receive()
    {
        OCTK_CHECK(!ptr_);
        return &ptr_;
    }

    Scoped &operator=(T *val)
    {
        OCTK_DCHECK(val);
        ptr_ = val;
        return *this;
    }

protected:
    T *ptr_ = nullptr;
};

template <>
Scoped<GError>::~Scoped();
template <>
Scoped<char>::~Scoped();
template <>
Scoped<GVariant>::~Scoped();
template <>
Scoped<GVariantIter>::~Scoped();
template <>
Scoped<GDBusMessage>::~Scoped();
template <>
Scoped<GUnixFDList>::~Scoped();

extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<GError>;
extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<char>;
extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<GVariant>;
extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<GVariantIter>;
extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<GDBusMessage>;
extern template class OCTK_EXPORT_TEMPLATE_DECLARE(OCTK_MEDIA_API) Scoped<GUnixFDList>;

} // namespace portal

OCTK_END_NAMESPACE