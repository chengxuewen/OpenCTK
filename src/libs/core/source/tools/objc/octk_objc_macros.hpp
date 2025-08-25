/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_OBJC_MACROS_HPP
#define _OCTK_OBJC_MACROS_HPP

#include <octk_global.hpp>

#define OCTK_OBJC_EXPORT OCTK_CORE_API

// Macro used to mark a function as deprecated.
#define OCTK_OBJC_DEPRECATED(msg) __attribute__((deprecated(msg)))

// Internal macros used to correctly concatenate symbols.
#define OCTK_SYMBOL_CONCAT_HELPER(a, b) a##b
#define OCTK_SYMBOL_CONCAT(a, b) OCTK_SYMBOL_CONCAT_HELPER(a, b)

// OCTK_OBJC_TYPE_PREFIX
//
// Macro used to prepend a prefix to the API types that are exported with
// OCTK_OBJC_EXPORT.
//
// Clients can patch the definition of this macro locally and build
// WebRTC.framework with their own prefix in case symbol clashing is a
// problem.
//
// This macro must be defined uniformily across all the translation units.
#ifndef OCTK_OBJC_TYPE_PREFIX
#   define OCTK_OBJC_TYPE_PREFIX
#endif

// RCT_OBJC_TYPE
//
// Macro used internally to declare API types. Declaring an API type without
// using this macro will not include the declared type in the set of types
// that will be affected by the configurable OCTK_OBJC_TYPE_PREFIX.
#define OCTK_OBJC_TYPE(type_name) OCTK_SYMBOL_CONCAT(OCTK_OBJC_TYPE_PREFIX, type_name)

#if defined(__cplusplus)
#   define OCTK_EXTERN extern "C" OCTK_OBJC_EXPORT
#else
#   define OCTK_EXTERN extern OCTK_OBJC_EXPORT
#endif

#ifdef __OBJC__
#   define OCTK_FWD_DECL_OBJC_CLASS(classname) @class classname
#else
#   define OCTK_FWD_DECL_OBJC_CLASS(classname) typedef struct objc_object classname
#endif

#endif  // _OCTK_OBJC_MACROS_HPP
