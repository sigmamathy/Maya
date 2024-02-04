#pragma once

#include "./graphics.hpp"

class MayaGraphics2D::Camera
{
public:

	Camera();

	void SetPosition(MayaFvec2 pos);
	void SetZoom(MayaFvec2 zoom);
	void SetRotation(float rot);

	MayaFvec2 GetPosition() const;
	MayaFvec2 GetZoom() const;
	float GetRotation() const;

	MayaFmat4 GetViewMatrix();

private:

	bool require_update = true;
	MayaFvec2 position;
	MayaFvec2 zoom;
	float rotation;
	MayaFmat4 view_matrix;

	friend class MayaGraphics2D;
};