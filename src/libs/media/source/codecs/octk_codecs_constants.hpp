/***********************************************************************************************************************
** Copyright (C) 2025~Present MaxSense, Created by ChengXueWen on 25-4-16.
***********************************************************************************************************************/

// This file contains constants that are used by multiple global
// codec definitions (modules/video_coding/codecs/*/include/*_globals.h)

#ifndef _OCTK_CODECS_CONSTANTS_HPP
#define _OCTK_CODECS_CONSTANTS_HPP

#include <octk_media_global.hpp>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

const int16_t kNoPictureId = -1;
const int16_t kNoTl0PicIdx = -1;
const uint8_t kNoTemporalIdx = 0xFF;
const int kNoKeyIdx = -1;

OCTK_END_NAMESPACE

#endif  // _OCTK_CODECS_CONSTANTS_HPP
