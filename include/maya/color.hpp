#pragma once

#include "./core.hpp"
#include "./math.hpp"

namespace maya
{

// ----------------- Color constants ----------------- //

constexpr Ivec4 Red			= { 255, 0, 0, 255 };
constexpr Ivec4 Green		= { 0, 255, 0, 255 };
constexpr Ivec4 Blue		= { 0, 0, 255, 255 };
constexpr Ivec4 Yellow		= { 255, 255, 0, 255 };
constexpr Ivec4 Cyan		= { 0, 255, 255, 255 };
constexpr Ivec4 Magenta		= { 255, 255, 0, 255 };
constexpr Ivec4 White		= { 255, 255, 255, 255 };
constexpr Ivec4 Black		= { 0, 0, 0, 255 };
constexpr Ivec4 Gray		= { 128, 128, 128, 255 };
constexpr Ivec4 DarkGray	= { 64, 64, 64, 255 };
constexpr Ivec4 LightGray	= { 192, 192, 192, 255 };

/**
	@brief Convert color hexcode to color vector.

	Convert hexcode in RGBA format into a RGBA vector if hasopacity = true,
	otherwise the hexcode will be seen as RGB format and automatically asssume
	the value of alpha is MAX (255).

	@param hexcode: color hexcode with RRGGBBAA layout or 00RRGGBB layout
	@param hasopacity: true if hexcode contains alpha value, by default false
*/
constexpr Ivec4 ConvertHexToColor(unsigned hexcode, bool hasopacity = false)
{
	Ivec4 result;
	if (hasopacity)
		result[0] = (hexcode >> 24) & 0xFF;
	result[0 + hasopacity] = (hexcode >> 16) & 0xFF;
	result[1 + hasopacity] = (hexcode >> 8) & 0xFF;
	result[2 + hasopacity] = hexcode & 0xFF;
	if (!hasopacity)
		result[3] = 255;
	return result;
}

}