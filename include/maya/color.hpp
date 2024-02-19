#pragma once

#include "./core.hpp"
#include "./math.hpp"

// ----------------- Color constants ----------------- //

constexpr MayaIvec4 MayaRed			= { 255, 0, 0, 255 };
constexpr MayaIvec4 MayaGreen		= { 0, 255, 0, 255 };
constexpr MayaIvec4 MayaBlue		= { 0, 0, 255, 255 };
constexpr MayaIvec4 MayaYellow		= { 255, 255, 0, 255 };
constexpr MayaIvec4 MayaCyan		= { 0, 255, 255, 255 };
constexpr MayaIvec4 MayaMagenta		= { 255, 255, 0, 255 };
constexpr MayaIvec4 MayaWhite		= { 255, 255, 255, 255 };
constexpr MayaIvec4 MayaBlack		= { 0, 0, 0, 255 };
constexpr MayaIvec4 MayaGray		= { 128, 128, 128, 255 };

/**
	@brief Convert color hexcode to color vector.

	Convert hexcode in RGBA format into a RGBA vector if hasopacity = true,
	otherwise the hexcode will be seen as RGB format and automatically asssume
	the value of alpha is MAX (255).

	@param hexcode: color hexcode with RRGGBBAA layout or 00RRGGBB layout
	@param hasopacity: true if hexcode contains alpha value, by default false
*/
MayaIvec4 MayaConvertHexToColor(unsigned hexcode, bool hasopacity = false);