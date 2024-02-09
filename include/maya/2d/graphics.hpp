#pragma once

#include "../core.hpp"
#include "../math.hpp"
#include "../renderer.hpp"
#include "../window.hpp"
#include "../font.hpp"
#include "../transformation.hpp"

class MayaGraphics2D
{
public:

	class Camera;
	class TextDisplay;

public:

	MayaGraphics2D(MayaWindow& window);

	MayaGraphics2D(MayaGraphics2D const&) = delete;

	MayaGraphics2D& operator=(MayaGraphics2D const&) = delete;

	MayaWindow* const Window;

	void UseProjection(float left, float right, float bottom, float top);

	void UseProjection(float width, float height);

	void UseProjection(MayaFvec2 size);

	void UseWindowProjection();

	void UseCamera(Camera* camera);

	void UseColor(int r, int g, int b, int a = 255);

	void UseColor(MayaIvec3 color);

	void UseColor(MayaIvec4 color);

	void UseColor(unsigned hexcode, bool hasopacity = false);

	void UseTexture(MayaTexture* texture);

	void DrawRect(float x, float y, float width, float height);

	void DrawRect(MayaFvec2 pos, MayaFvec2 size);

	void DrawRectBorder(float x, float y, float width, float height, int linewidth = 1);

	void DrawRectBorder(MayaFvec2 pos, MayaFvec2 size, int linewidth = 1);

	void DrawLine(float startx, float starty, float endx, float endy);

	void DrawLine(MayaFvec2 start, MayaFvec2 end);

	void DrawOval(float x, float y, float width, float height);

	void DrawOval(MayaFvec2 pos, MayaFvec2 size);

	void DrawText(MayaFont& font, MayaStringCR text, float x, float y, MayaCorner align = MayaCornerBC);

	void DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos, MayaCorner align = MayaCornerBC);

	void DrawText(TextDisplay& text);

	void DrawText(TextDisplay& text, int start, int end);

private:

	MayaShaderProgramUptr program;
	MayaVertexArrayUptr squarevao, circlevao64;
	Camera* camera;
	MayaTexture* texture;
};