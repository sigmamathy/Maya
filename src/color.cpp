#include <maya/color.hpp>

MayaIvec4 MayaConvertHexToColor(unsigned hexcode, bool hasopacity)
{
	MayaIvec4 result;
	if (hasopacity)
		result[0] = (hexcode >> 24) & 0xFF;
	result[0 + hasopacity] = (hexcode >> 16) & 0xFF;
	result[1 + hasopacity] = (hexcode >> 8) & 0xFF;
	result[2 + hasopacity] = hexcode & 0xFF;
	if (!hasopacity)
		result[3] = 255;
	return result;
}