#pragma once

#include "./core.hpp"
#include "./math.hpp"

// Specify a corner for aligning
// Horizontal:      (x & 0b11) = 1, 2, 3 for Left, Center, Right
// Vertical:        (x >> 2) = 1, 2, 3 for Bottom, Center, Top
enum MayaCorner
{
	MayaCornerLeft		= 0b1,			// 0001
	MayaCornerCenterH	= 0b10,			// 0010
	MayaCornerRight		= 0b11,			// 0011
	MayaCornerBottom	= 0b100,		// 0100
	MayaCornerCenterV	= 0b1000,		// 1000
	MayaCornerTop		= 0b1100,		// 1100

// ---------------- Combinations ---------------- //

	MayaCornerTL = MayaCornerTop | MayaCornerLeft,
	MayaCornerTC = MayaCornerTop | MayaCornerCenterH,
	MayaCornerTR = MayaCornerTop | MayaCornerRight,
	MayaCornerCL = MayaCornerCenterV | MayaCornerLeft,
	MayaCornerCC = MayaCornerCenterV | MayaCornerCenterH,
	MayaCornerCR = MayaCornerCenterV | MayaCornerRight,
	MayaCornerBL = MayaCornerBottom | MayaCornerLeft,
	MayaCornerBC = MayaCornerBottom | MayaCornerCenterH,
	MayaCornerBR = MayaCornerBottom | MayaCornerRight,
};

// Translation matrix (2D)
MayaFmat4 MayaTranslate(MayaFvec2 position);

// Translation matrix
MayaFmat4 MayaTranslate(MayaFvec3 position);

// Rotation matrix (rotate about an axis)
MayaFmat4 MayaRotate(float rad, MayaFvec3 axis = MayaFvec3(0.0f, 0.0f, -1.0f));

// Scale matrix (2D)
MayaFmat4 MayaScale(MayaFvec2 scale);

// Scale matrix
MayaFmat4 MayaScale(MayaFvec3 scale);

// 3 dimensional view matrix, look at a certain position
MayaFmat4 MayaLookAt(MayaFvec3 position, MayaFvec3 orientation, MayaFvec3 up = MayaFvec3(0.0f, 1.0f, 0.0f));

// Orthogonal projection matrix
MayaFmat4 MayaOrthogonalProjection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

// Orthogonal projection matrix with center origin
MayaFmat4 MayaOrthogonalProjection(MayaFvec2 size);

// Perspective projection matrix
MayaFmat4 MayaPerspectiveProjection(float fovy, float aspect, float near, float far);