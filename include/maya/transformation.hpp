#pragma once

#include "./core.hpp"
#include "./math.hpp"

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

// Perspective projection matrix
MayaFmat4 MayaPerspectiveProjection(float fovy, float aspect, float near, float far);