#pragma once

#include "./math.hpp"

struct MayaBlending
{
	enum BlendFactor
	{
		Zero = 0,
		One = 1,
		SrcColor			= 0x0300,
		OneMinusSrcColor	= 0x0301,
		SrcAlpha			= 0x0302,
		OneMinusSrcAlpha	= 0x0303,
		DstAlpha			= 0x0304,
		OneMinusDstAlpha	= 0x0305,
		DstColor			= 0x0306,
		OneMinusDstColor	= 0x0307,
	};

	BlendFactor SrcFactor;
	BlendFactor DestFactor;
};

struct MayaScissorTest
{
	MayaIvec2 Position;
	MayaIvec2 Size;
};