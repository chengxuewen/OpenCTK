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

#ifndef _OCTK_DELEGATED_SOURCE_LIST_CONTROLLER_HPP
#define _OCTK_DELEGATED_SOURCE_LIST_CONTROLLER_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// A controller to be implemented and returned by
// GetDelegatedSourceListController in capturers that require showing their own
// source list and managing user selection there. Apart from ensuring the
// visibility of the source list, these capturers should largely be interacted
// with the same as a normal capturer, though there may be some caveats for
// some DesktopCapturer methods. See GetDelegatedSourceListController for more
// information.
class OCTK_MEDIA_API DelegatedSourceListController
{
public:
    // Notifications that can be used to help drive any UI that the consumer may
    // want to show around this source list (e.g. if an consumer shows their own
    // UI in addition to the delegated source list).
    class Observer
    {
    public:
        // Called after the user has made a selection in the delegated source list.
        // Note that the consumer will still need to get the source out of the
        // capturer by calling GetSourceList.
        virtual void OnSelection() = 0;

        // Called when there is any user action that cancels the source selection.
        virtual void OnCancelled() = 0;

        // Called when there is a system error that cancels the source selection.
        virtual void OnError() = 0;

    protected:
        virtual ~Observer() {}
    };

    // Observer must remain valid until the owning DesktopCapturer is destroyed.
    // Only one Observer is allowed at a time, and may be cleared by passing
    // nullptr.
    virtual void Observe(Observer* observer) = 0;

    // Used to prompt the capturer to show the delegated source list. If the
    // source list is already visible, this will be a no-op. Must be called after
    // starting the DesktopCapturer.
    //
    // Note that any selection from a previous invocation of the source list may
    // be cleared when this method is called.
    virtual void EnsureVisible() = 0;

    // Used to prompt the capturer to hide the delegated source list. If the
    // source list is already hidden, this will be a no-op.
    virtual void EnsureHidden() = 0;

protected:
    virtual ~DelegatedSourceListController() {}
};

OCTK_END_NAMESPACE

#endif // _OCTK_DELEGATED_SOURCE_LIST_CONTROLLER_HPP
