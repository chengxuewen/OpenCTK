/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2018 The WebRTC project authors.
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

#include <mac/octk_desktop_frame_cgimage.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <AvailabilityMacros.h>

OCTK_BEGIN_NAMESPACE

// static
std::unique_ptr<DesktopFrameCGImage> DesktopFrameCGImage::CreateForDisplay(CGDirectDisplayID display_id)
{
    // Create an image containing a snapshot of the display.
    ScopedCFTypeRef<CGImageRef> cg_image(CGDisplayCreateImage(display_id));
    if (!cg_image)
    {
        return nullptr;
    }

    return DesktopFrameCGImage::CreateFromCGImage(cg_image);
}

// static
std::unique_ptr<DesktopFrameCGImage> DesktopFrameCGImage::CreateForWindow(CGWindowID window_id)
{
    ScopedCFTypeRef<CGImageRef> cg_image(
        CGWindowListCreateImage(CGRectNull,
                                kCGWindowListOptionIncludingWindow,
                                window_id,
                                kCGWindowImageBoundsIgnoreFraming));
    if (!cg_image)
    {
        return nullptr;
    }

    return DesktopFrameCGImage::CreateFromCGImage(cg_image);
}

// static
std::unique_ptr<DesktopFrameCGImage> DesktopFrameCGImage::CreateFromCGImage(ScopedCFTypeRef<CGImageRef> cg_image)
{
    // Verify that the image has 32-bit depth.
    int bits_per_pixel = CGImageGetBitsPerPixel(cg_image.get());
    if (bits_per_pixel / 8 != DesktopFrame::kBytesPerPixel)
    {
        OCTK_ERROR() << "CGDisplayCreateImage() returned imaged with " << bits_per_pixel
                     << " bits per pixel. Only 32-bit depth is supported.";
        return nullptr;
    }

    // Request access to the raw pixel data via the image's DataProvider.
    CGDataProviderRef cg_provider = CGImageGetDataProvider(cg_image.get());
    OCTK_DCHECK(cg_provider);

    // CGDataProviderCopyData returns a new data object containing a copy of the provider’s
    // data.
    ScopedCFTypeRef<CFDataRef> cg_data(CGDataProviderCopyData(cg_provider));
    OCTK_DCHECK(cg_data);

    // CFDataGetBytePtr returns a read-only pointer to the bytes of a CFData object.
    uint8_t *data = const_cast<uint8_t *>(CFDataGetBytePtr(cg_data.get()));
    OCTK_DCHECK(data);

    DesktopSize size(CGImageGetWidth(cg_image.get()), CGImageGetHeight(cg_image.get()));
    int stride = CGImageGetBytesPerRow(cg_image.get());

    std::unique_ptr<DesktopFrameCGImage> frame(
        new DesktopFrameCGImage(size, stride, data, cg_image, cg_data));

    CGColorSpaceRef cg_color_space = CGImageGetColorSpace(cg_image.get());
    if (cg_color_space)
    {
#if !defined(MAC_OS_X_VERSION_10_13) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_13
        ScopedCFTypeRef<CFDataRef> cf_icc_profile(CGColorSpaceCopyICCProfile(cg_color_space));
#else
        ScopedCFTypeRef<CFDataRef> cf_icc_profile(CGColorSpaceCopyICCData(cg_color_space));
#endif
        if (cf_icc_profile)
        {
            const uint8_t *data_as_byte =
                reinterpret_cast<const uint8_t *>(CFDataGetBytePtr(cf_icc_profile.get()));
            const size_t data_size = CFDataGetLength(cf_icc_profile.get());
            if (data_as_byte && data_size > 0)
            {
                frame->set_icc_profile(std::vector<uint8_t>(data_as_byte, data_as_byte + data_size));
            }
        }
    }

    return frame;
}

DesktopFrameCGImage::DesktopFrameCGImage(DesktopSize size,
                                         int stride,
                                         uint8_t *data,
                                         ScopedCFTypeRef<CGImageRef> cg_image,
                                         ScopedCFTypeRef<CFDataRef> cg_data)
    : DesktopFrame(size, stride, data, nullptr), cg_image_(cg_image), cg_data_(cg_data)
{
    OCTK_DCHECK(cg_image_);
    OCTK_DCHECK(cg_data_);
}

DesktopFrameCGImage::~DesktopFrameCGImage() = default;
OCTK_END_NAMESPACE
