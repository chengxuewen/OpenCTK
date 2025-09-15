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

#ifndef _OCTK_IMGUI_APPLICATION_SDLGPU3_HPP
#define _OCTK_IMGUI_APPLICATION_SDLGPU3_HPP

#include <octk_imgui_application.hpp>

OCTK_BEGIN_NAMESPACE

class ImGuiApplicationSDLGPU3Private;
class ImGuiApplicationSDLGPU3 : public ImGuiApplication
{
public:
    ImGuiApplicationSDLGPU3(const Properties &properties = {});
    ~ImGuiApplicationSDLGPU3() override;

    bool exec() override;
    StringView typeName() const override;
    ImGuiImageResult createImage(ImGuiImage::Format format, const Binary &binary, int width, int height) override;

protected:
    bool init() override;
    void destroy() override;

private:
    OCTK_DECLARE_PRIVATE(ImGuiApplicationSDLGPU3)
    OCTK_DISABLE_COPY_MOVE(ImGuiApplicationSDLGPU3)
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_APPLICATION_SDLGPU3_HPP
