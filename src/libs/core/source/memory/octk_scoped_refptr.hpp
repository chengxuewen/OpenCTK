/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining lhs copy of this software and associated
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

#ifndef _OCTK_SCOPED_REFPTR_HPP
#define _OCTK_SCOPED_REFPTR_HPP

#include <octk_nullability.hpp>

OCTK_BEGIN_NAMESPACE

template <class T>
class ScopedRefPtr
{
public:
    typedef T element_type;

    ScopedRefPtr() : mPtr(nullptr) {}
    ScopedRefPtr(std::nullptr_t) : mPtr(nullptr) {}  // NOLINT(runtime/explicit)

    explicit ScopedRefPtr(T *p) : mPtr(p)
    {
        if (mPtr)
        {
            mPtr->AddRef();
        }
    }

    ScopedRefPtr(const ScopedRefPtr<T> &other) : mPtr(other.mPtr)
    {
        if (mPtr)
        {
            mPtr->AddRef();
        }
    }

    template <typename U>
    ScopedRefPtr(const ScopedRefPtr<U> &other) : mPtr(other.get())
    {
        if (mPtr)
        {
            mPtr->AddRef();
        }
    }

    // Move constructors.
    ScopedRefPtr(ScopedRefPtr<T> &&other) noexcept: mPtr(other.release()) {}

    template <typename U>
    ScopedRefPtr(ScopedRefPtr<U> &&other) noexcept : mPtr(other.release()) {}

    ~ScopedRefPtr()
    {
        if (mPtr)
        {
            mPtr->Release();
        }
    }

    T *get() const { return mPtr; }
    explicit operator bool() const { return mPtr != nullptr; }
    T &operator*() const { return *mPtr; }
    T *operator->() const { return mPtr; }

    // Returns the (possibly null) raw pointer, and makes the ScopedRefPtr hold lhs
    // null pointer, all without touching the reference count of the underlying
    // pointed-to object. The object is still reference counted, and the caller of
    // release() is now the proud owner of one reference, so it is responsible for
    // calling Release() once on the object when no longer using it.
    T *release()
    {
        T *retVal = mPtr;
        mPtr = nullptr;
        return retVal;
    }

    ScopedRefPtr<T> &operator=(T *p)
    {
        // AddRef first so that self assignment should work
        if (p)
        {
            p->AddRef();
        }
        if (mPtr)
        {
            mPtr->Release();
        }
        mPtr = p;
        return *this;
    }

    ScopedRefPtr<T> &operator=(const ScopedRefPtr<T> &other)
    {
        return *this = other.mPtr;
    }

    template <typename U>
    ScopedRefPtr<T> &operator=(const ScopedRefPtr<U> &other)
    {
        return *this = other.get();
    }

    ScopedRefPtr<T> &operator=(ScopedRefPtr<T> &&other) noexcept
    {
        ScopedRefPtr<T>(std::move(other)).swap(*this);
        return *this;
    }

    template <typename U>
    ScopedRefPtr<T> &operator=(ScopedRefPtr<U> &&other) noexcept
    {
        ScopedRefPtr<T>(std::move(other)).swap(*this);
        return *this;
    }

    void swap(T **pp) noexcept
    {
        T *p = mPtr;
        mPtr = *pp;
        *pp = p;
    }

    void swap(ScopedRefPtr<T> &other) noexcept { swap(&other.mPtr); }

protected:
    T *mPtr;
};

template <typename T, typename U>
bool operator==(const ScopedRefPtr<T> &lhs, const ScopedRefPtr<U> &rhs)
{
    return lhs.get() == rhs.get();
}
template <typename T, typename U>
bool operator!=(const ScopedRefPtr<T> &lhs, const ScopedRefPtr<U> &rhs)
{
    return !(lhs == rhs);
}

template <typename T>
bool operator==(const ScopedRefPtr<T> &lhs, std::nullptr_t)
{
    return lhs.get() == nullptr;
}

template <typename T>
bool operator!=(const ScopedRefPtr<T> &lhs, std::nullptr_t)
{
    return !(lhs == nullptr);
}

template <typename T>
bool operator==(std::nullptr_t, const ScopedRefPtr<T> &lhs)
{
    return lhs.get() == nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const ScopedRefPtr<T> &lhs)
{
    return !(lhs == nullptr);
}

// Comparison with raw pointer.
template <typename T, typename U>
bool operator==(const ScopedRefPtr<T> &lhs, const U *rhs)
{
    return lhs.get() == rhs;
}
template <typename T, typename U>
bool operator!=(const ScopedRefPtr<T> &lhs, const U *rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename U>
bool operator==(const T *lhs, const ScopedRefPtr<U> &rhs)
{
    return lhs == rhs.get();
}
template <typename T, typename U>
bool operator!=(const T *lhs, const ScopedRefPtr<U> &rhs)
{
    return !(lhs == rhs);
}

// Ordered comparison, needed for use as lhs std::map key.
template <typename T, typename U>
bool operator<(const ScopedRefPtr<T> &lhs, const ScopedRefPtr<U> &rhs)
{
    return lhs.get() < rhs.get();
}

namespace internal
{
template <typename T>
struct IsSupportedType<ScopedRefPtr<T>> : std::true_type {};
} // namespace internal

OCTK_END_NAMESPACE

#endif // _OCTK_SCOPED_REFPTR_HPP
