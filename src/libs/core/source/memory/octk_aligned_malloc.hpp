/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2011 The WebRTC project authors.
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


#ifndef _OCTK_ALIGNED_MALLOC_HPP
#define _OCTK_ALIGNED_MALLOC_HPP

#include <octk_global.hpp>

// The functions declared here
// 1) Allocates block of aligned memory.
// 2) Re-calculates a pointer such that it is aligned to a higher or equal
//    address.
// Note: alignment must be a power of two. The alignment is in bytes.

#include <stddef.h>

OCTK_BEGIN_NAMESPACE
namespace utils
{
// Returns a pointer to the first boundry of `alignment` bytes following the
// address of `ptr`.
// Note that there is no guarantee that the memory in question is available.
// `ptr` has no requirements other than it can't be NULL.
OCTK_CORE_API void *getRightAlign(const void *ptr, size_t alignment);

// Allocates memory of `size` bytes aligned on an `alignment` boundry.
// The return value is a pointer to the memory. Note that the memory must
// be de-allocated using alignedFree.
OCTK_CORE_API void *alignedMalloc(size_t size, size_t alignment);
// De-allocates memory created using the alignedMalloc() API.
OCTK_CORE_API void alignedFree(void *mem_block);

// Templated versions to facilitate usage of aligned malloc without casting
// to and from void*.
template <typename T>
T *getRightAlign(const T *ptr, size_t alignment)
{
    return reinterpret_cast<T *>(getRightAlign(reinterpret_cast<const void *>(ptr), alignment));
}
template <typename T>
T *alignedMalloc(size_t size, size_t alignment)
{
    return reinterpret_cast<T *>(alignedMalloc(size, alignment));
}
} // namespace utils
// Deleter for use with unique_ptr. E.g., use as
//   std::unique_ptr<Foo, alignedFreeDeleter> foo;
struct AlignedFreeDeleter
{
    inline void operator()(void *ptr) const { utils::alignedFree(ptr); }
};
OCTK_END_NAMESPACE

#endif  // _OCTK_ALIGNED_MALLOC_HPP
