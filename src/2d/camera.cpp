#include <maya/2d/camera.hpp>
#include <maya/transformation.hpp>

MayaCamera2d::MayaCamera2d()
	: position(0), zoom(1), rotation(0), view_matrix(1), require_update(0)
{
}

void MayaCamera2d::SetPosition(MayaFvec2 pos)
{
	this->position = pos;
	require_update = true;
}

void MayaCamera2d::SetZoom(MayaFvec2 zoom)
{
	this->zoom = zoom;
	require_update = true;
}

void MayaCamera2d::SetRotation(float rot)
{
	this->rotation = rot;
	require_update = true;
}

MayaFvec2 MayaCamera2d::GetPosition() const
{
	return position;
}

MayaFvec2 MayaCamera2d::GetZoom() const
{
	return zoom;
}

float MayaCamera2d::GetRotation() const
{
	return rotation;
}

MayaFmat4 MayaCamera2d::GetViewMatrix()
{
	if (require_update)
		view_matrix = MayaScale(zoom) * MayaRotate(rotation) * MayaTranslate(position);
	require_update = false;
	return view_matrix;
}