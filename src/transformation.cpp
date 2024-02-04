#include <maya/transformation.hpp>

MayaFmat4 MayaTranslate(MayaFvec2 position)
{
	return MayaTranslate(MayaFvec3(position.x, position.y, 0.0f));
}

MayaFmat4 MayaTranslate(MayaFvec3 position)
{
	return MayaFmat4(
		1.0f, 0.0f, 0.0f, position.x,
		0.0f, 1.0f, 0.0f, position.y,
		0.0f, 0.0f, 1.0f, position.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

MayaFmat4 MayaRotate(float rad, MayaFvec3 axis)
{
	if (rad == 0)
		return MayaFmat4(1);
	axis = MayaNormalize(axis);
	float c = std::cos(rad), s = std::sin(rad);
	auto fun1 = [&](int i) -> float { return c + axis[i] * axis[i] * (1 - c); };
	auto fun2 = [&](int i, int j, int k) -> float { return (1 - c) * axis[i] * axis[j] + s * axis[k]; };
	auto fun3 = [&](int i, int j, int k) -> float { return (1 - c) * axis[i] * axis[j] - s * axis[k]; };
	return MayaFmat4(
		fun1(0), fun3(0, 1, 2), fun2(0, 2, 1), 0.0f,
		fun2(0, 1, 2), fun1(1), fun3(1, 2, 0), 0.0f,
		fun3(0, 2, 1), fun2(1, 2, 0), fun1(2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

MayaFmat4 MayaScale(MayaFvec2 scale)
{
	return MayaScale(MayaFvec3(scale[0], scale[1], 1.0f));
}

MayaFmat4 MayaScale(MayaFvec3 scale)
{
	return MayaFmat4(
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

MayaFmat4 MayaLookAt(MayaFvec3 position, MayaFvec3 orientation, MayaFvec3 up)
{
	MayaFmat4 res(1.0f);
	MayaFvec3 f = MayaNormalize(orientation);
	MayaFvec3 s = MayaNormalize(MayaCross(f, up));
	MayaFvec3 u = MayaCross(s, f);
	res[0][0] = s[0];
	res[1][0] = s[1];
	res[2][0] = s[2];
	res[0][1] = u[0];
	res[1][1] = u[1];
	res[2][1] = u[2];
	res[0][2] = -f[0];
	res[1][2] = -f[1];
	res[2][2] = -f[2];
	res[3][0] = -MayaDot(s, position);
	res[3][1] = -MayaDot(u, position);
	res[3][2] = MayaDot(f, position);
	return res;
}

MayaFmat4 MayaOrthogonalProjection(float left, float right, float bottom, float top, float near, float far)
{
	float x = right - left,
		y = top - bottom,
		z = far - near;
	return MayaFmat4(
		2.0f / x, 0.0f, 0.0f, -(right + left) / x,
		0.0f, 2.0f / y, 0.0f, -(top + bottom) / y,
		0.0f, 0.0f, -2.0f / z, -(far + near) / z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

MayaFmat4 MayaOrthogonalProjection(MayaFvec2 size)
{
	return MayaOrthogonalProjection(
		-size[0] / 2.0f, size[0] / 2.0f, -size[1] / 2.0f, size[1] / 2.0f);
}

MayaFmat4 MayaPerspectiveProjection(float fovy, float aspect, float near, float far)
{
	MayaFmat4 res(0.0f);
	float const tanHalfFovy = tan(fovy / 2.0f);
	res[0][0] = 1.0f / (-aspect * tanHalfFovy);
	res[1][1] = 1.0f / (tanHalfFovy);
	res[2][2] = -(far + near) / (far - near);
	res[2][3] = -1.0f;
	res[3][2] = -(2.0f * far * near) / (far - near);
	return res;
}