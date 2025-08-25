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

#ifndef _OCTK_MOVE_WRAPPER_HPP
#define _OCTK_MOVE_WRAPPER_HPP

#include <octk_global.hpp>

OCTK_BEGIN_NAMESPACE

/**
 * @brief C++11 closures don't support move-in capture. Nor does std::bind. facepalm.
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3610.html
 *
 * "[...] a work-around that should make people's stomach crawl:
 *  write a wrapper that performs move-on-copy, much like the deprecated auto_ptr"
 *  Unlike auto_ptr, this doesn't require a heap allocation.
 */
template <typename T>
class MoveWrapper
{
public:
    /**
     * @brief If value can be default-constructed, why not? Then we don't have to move it in
     */
    MoveWrapper() = default;
    explicit MoveWrapper(T &&t) : mValue(std::move(t)) {}
    /**
     * @brief If you want these you're probably doing it wrong, though they'd be easy enough to implement
     */
    MoveWrapper &operator=(MoveWrapper const &) = delete;
    MoveWrapper &operator=(MoveWrapper &&) = delete;
    /// copy is move
    MoveWrapper(const MoveWrapper &other) : mValue(std::move(other.mValue)) {}
    /// move is also move
    MoveWrapper(MoveWrapper &&other) : mValue(std::move(other.mValue)) {}

    const T &operator*() const { return mValue; }
    T &operator*() { return mValue; }

    const T *operator->() const { return &mValue; }
    T *operator->() { return &mValue; }

    const T &ref() const { return mValue; }
    T &ref() { return mValue; }

    const T *get() const { return &mValue; }
    T *get() { return &mValue; }

    /// move the value out (sugar for std::move(*moveWrapper))
    T &&move() { return std::move(mValue); }

private:
    mutable T mValue;
};

namespace utils
{
/// Make a MoveWrapper from the argument. Because the name "makeMoveWrapper"
/// is already quite transparent in its intent, this will work for lvalues as
/// if you had wrapped them in std::move.
template <typename T, typename T0 = typename std::remove_reference<T>::type>
MoveWrapper<T0> makeMoveWrapper(T &&t)
{
    return MoveWrapper<T0>(std::forward<T0>(t));
}
}
OCTK_END_NAMESPACE

#endif // _OCTK_MOVE_WRAPPER_HPP
