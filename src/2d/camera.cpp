#include <maya/2d/camera.hpp>
#include <maya/transformation.hpp>

MayaGraphics2D::Camera::Camera()
	: position(0), zoom(1), rotation(0), view_matrix(1), require_update(0)
{
}

void MayaGraphics2D::Camera::SetPosition(MayaFvec2 pos)
{
	this->position = pos;
	require_update = true;
}

void MayaGraphics2D::Camera::SetZoom(MayaFvec2 zoom)
{
	this->zoom = zoom;
	require_update = true;
}

void MayaGraphics2D::Camera::SetRotation(float rot)
{
	this->rotation = rot;
	require_update = true;
}

MayaFvec2 MayaGraphics2D::Camera::GetPosition() const
{
	return position;
}

MayaFvec2 MayaGraphics2D::Camera::GetZoom() const
{
	return zoom;
}

float MayaGraphics2D::Camera::GetRotation() const
{
	return rotation;
}

MayaFmat4 MayaGraphics2D::Camera::GetViewMatrix()
{
	if (require_update)
		view_matrix = MayaScale(zoom) * MayaRotate(rotation) * MayaTranslate(position);
	return view_matrix;
}