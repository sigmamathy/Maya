#pragma once

#include "../core.hpp"
#include "../math.hpp"
#include "../renderer.hpp"
#include "../window.hpp"

class MayaGraphics2D
{
public:

	MayaWindow* const Window;

	void UseProjection(float width, float height);

	void UseProjection(MayaFvec2 size);

	void UseWindowProjection();

	void UseColor(int r, int g, int b, int a = 255);

	void UseColor(MayaIvec3 color);

	void UseColor(MayaIvec4 color);

	void UseColor(unsigned hexcode, bool hasopacity = false);

	void UseTexture(MayaTexture* texture);

	void DrawRect(float x, float y, float width, float height);

	void DrawRect(MayaFvec2 pos, MayaFvec2 size);

public:

	class Camera;
	class Renderer;

private:

	MayaGraphics2D(MayaWindow* window);

	MayaGraphics2D(MayaGraphics2D const&) = delete;

	MayaGraphics2D& operator=(MayaGraphics2D const&) = delete;

	friend class MayaWindow;

private:

	MayaShaderProgramUptr program;
	MayaVertexArrayUptr squarevao;
	MayaTexture* texture;

	friend class Camera;
	friend class Renderer;

};