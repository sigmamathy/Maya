#pragma once

#include "./math.hpp"

struct MayaBlending
{
	enum BlendFunc {
		OneMinusSrcAlpha = 0x0303
	} Func;
};

struct MayaScissorTest
{
	MayaIvec2 Position;
	MayaIvec2 Size;
};