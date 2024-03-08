#pragma once

#include "./core.hpp"
#include "./math.hpp"

namespace maya
{

// Specify a corner for aligning
// Horizontal:      (x & 0b11) = 1, 2, 3 for Left, Center, Right
// Vertical:        (x >> 2) = 1, 2, 3 for Bottom, Center, Top
enum Alignment
{
	AlignLeft		= 0b1,			// 0001
	AlignCenterH	= 0b10,			// 0010
	AlignRight		= 0b11,			// 0011
	AlignBottom		= 0b100,		// 0100
	AlignCenterV	= 0b1000,		// 1000
	AlignTop		= 0b1100,		// 1100

	// ---------------- Combinations ---------------- //

	AlignTL = AlignTop | AlignLeft,
	AlignTC = AlignTop | AlignCenterH,
	AlignTR = AlignTop | AlignRight,
	AlignCL = AlignCenterV | AlignLeft,
	AlignCC = AlignCenterV | AlignCenterH,
	AlignCR = AlignCenterV | AlignRight,
	AlignBL = AlignBottom | AlignLeft,
	AlignBC = AlignBottom | AlignCenterH,
	AlignBR = AlignBottom | AlignRight,
};

// Translation matrix (2D)
Fmat4 TranslateModel(Fvec2 position);

// Translation matrix
Fmat4 TranslateModel(Fvec3 position);

// Rotation matrix (rotate about an axis)
Fmat4 RotateModel(float rad, Fvec3 axis = Fvec3(0.0f, 0.0f, -1.0f));

// Scale matrix (2D)
Fmat4 ScaleModel(Fvec2 scale);

// Scale matrix
Fmat4 ScaleModel(Fvec3 scale);

// 3 dimensional view matrix, look at a certain position
Fmat4 LookAtView(Fvec3 position, Fvec3 orientation, Fvec3 up = Fvec3(0.0f, 1.0f, 0.0f));

// Orthogonal projection matrix
Fmat4 OrthogonalProjection(float left, float right, float bottom, float top, float near = -1.0f, float far = 1.0f);

// Orthogonal projection matrix with center origin
Fmat4 OrthogonalProjection(Fvec2 size);

// Perspective projection matrix
Fmat4 PerspectiveProjection(float fovy, float aspect, float near, float far);

}