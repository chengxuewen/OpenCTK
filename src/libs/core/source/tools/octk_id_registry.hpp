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

#ifndef _OCTK_ID_REGISTRY_HPP
#define _OCTK_ID_REGISTRY_HPP

#include <octk_global.hpp>

OCTK_BEGIN_NAMESPACE

class IdRegistryPrivate;
class OCTK_CORE_API IdRegistry
{
public:
    IdRegistry();
    virtual ~IdRegistry();

    int64_t registeredIdCount() const;
    bool isIdRegistered(int64_t id) const;

    int64_t requestId();
    void registerId(int64_t id);
    void unregisterId(int64_t id);

protected:
    OCTK_DEFINE_DPTR(IdRegistry)
    OCTK_DECLARE_PRIVATE(IdRegistry)
    OCTK_DISABLE_COPY_MOVE(IdRegistry)
};

OCTK_END_NAMESPACE

#endif // _OCTK_ID_REGISTRY_HPP
