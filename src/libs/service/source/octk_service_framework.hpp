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

#ifndef _OCTK_SERVICE_FRAMEWORK_HPP
#define _OCTK_SERVICE_FRAMEWORK_HPP

#include <octk_service_global.hpp>

OCTK_BEGIN_NAMESPACE

class ServiceFrameworkPrivate;
class OCTK_SERVICE_API ServiceFramework
{
public:
    explicit ServiceFramework();
    virtual ~ServiceFramework();

    void init();

    void start();
    void stop();

protected:
    OCTK_DEFINE_DPTR(ServiceFramework)
    OCTK_DECLARE_PRIVATE(ServiceFramework)
    OCTK_DISABLE_COPY_MOVE(ServiceFramework)
};

OCTK_END_NAMESPACE

#endif // _OCTK_SERVICE_FRAMEWORK_HPP
