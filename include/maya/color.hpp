#pragma once

#include "./core.hpp"
#include "./math.hpp"

static constexpr MayaIvec4 MayaRed			= { 255, 0, 0, 255 };
static constexpr MayaIvec4 MayaGreen		= { 0, 255, 0, 255 };
static constexpr MayaIvec4 MayaBlue			= { 0, 0, 255, 255 };
static constexpr MayaIvec4 MayaYellow		= { 255, 255, 0, 255 };
static constexpr MayaIvec4 MayaCyan			= { 0, 255, 255, 255 };
static constexpr MayaIvec4 MayaMagenta		= { 255, 255, 0, 255 };
static constexpr MayaIvec4 MayaWhite		= { 255, 255, 255, 255 };
static constexpr MayaIvec4 MayaBlack		= { 0, 0, 0, 255 };
static constexpr MayaIvec4 MayaGray			= { 128, 128, 128, 255 };

MayaIvec4 MayaConvertHexToColor(unsigned hexcode, bool hasopacity = false);