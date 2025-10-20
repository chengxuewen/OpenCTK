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

#ifndef _OCTK_SCOPED_CFTYPE_REF_HPP
#define _OCTK_SCOPED_CFTYPE_REF_HPP

#include <octk_global.hpp>

#include <CoreFoundation/CoreFoundation.h>

OCTK_BEGIN_NAMESPACE

// RETAIN: ScopedTypeRef should retain the object when it takes
// ownership.
// ASSUME: Assume the object already has already been retained.
// ScopedTypeRef takes over ownership.
enum class RetainPolicy { RETAIN, ASSUME };

namespace detail
{
template <typename T>
struct CFTypeRefTraits
{
    static T InvalidValue() { return nullptr; }
    static void Release(T ref) { CFRelease(ref); }
    static T Retain(T ref)
    {
        CFRetain(ref);
        return ref;
    }
};

template <typename T, typename Traits>
class ScopedTypeRef
{
public:
    ScopedTypeRef() : ptr_(Traits::InvalidValue()) {}
    explicit ScopedTypeRef(T ptr) : ptr_(ptr) {}
    ScopedTypeRef(T ptr, RetainPolicy policy) : ScopedTypeRef(ptr)
    {
        if (ptr_ && policy == RetainPolicy::RETAIN)
        {
            Traits::Retain(ptr_);
        }
    }

    ScopedTypeRef(const ScopedTypeRef<T, Traits> &rhs) : ptr_(rhs.ptr_)
    {
        if (ptr_)
        {
            ptr_ = Traits::Retain(ptr_);
        }
    }

    ~ScopedTypeRef()
    {
        if (ptr_)
        {
            Traits::Release(ptr_);
        }
    }

    T get() const { return ptr_; }
    T operator->() const { return ptr_; }
    explicit operator bool() const { return ptr_; }

    bool operator!() const { return !ptr_; }

    ScopedTypeRef &operator=(const T &rhs)
    {
        if (ptr_)
        {
            Traits::Release(ptr_);
        }
        ptr_ = rhs;
        return *this;
    }

    ScopedTypeRef &operator=(const ScopedTypeRef<T, Traits> &rhs)
    {
        reset(rhs.get(), RetainPolicy::RETAIN);
        return *this;
    }

    // This is intended to take ownership of objects that are
    // created by pass-by-pointer initializers.
    T *InitializeInto()
    {
        RTC_DCHECK(!ptr_);
        return &ptr_;
    }

    void reset(T ptr, RetainPolicy policy = RetainPolicy::ASSUME)
    {
        if (ptr && policy == RetainPolicy::RETAIN)
        {
            Traits::Retain(ptr);
        }
        if (ptr_)
        {
            Traits::Release(ptr_);
        }
        ptr_ = ptr;
    }

    T release()
    {
        T temp = ptr_;
        ptr_ = Traits::InvalidValue();
        return temp;
    }

private:
    T ptr_;
};
}  // namespace detail

template <typename T>
using ScopedCFTypeRef = detail::ScopedTypeRef<T, detail::CFTypeRefTraits<T>>;

template <typename T>
static ScopedCFTypeRef<T> AdoptCF(T cftype)
{
    return ScopedCFTypeRef<T>(cftype, RetainPolicy::RETAIN);
}

template <typename T>
static ScopedCFTypeRef<T> ScopedCF(T cftype)
{
    return ScopedCFTypeRef<T>(cftype);
}

OCTK_END_NAMESPACE

#endif // _OCTK_SCOPED_CFTYPE_REF_HPP
