#pragma once

#include "../core.hpp"
#include "../math.hpp"
#include "../renderer.hpp"
#include "../window.hpp"
#include "../font.hpp"
#include "../transformation.hpp"

class MayaGraphics2d
{
public:

	MayaGraphics2d(MayaWindow& window);
	MayaGraphics2d(MayaGraphics2d const&) = delete;
	MayaGraphics2d& operator=(MayaGraphics2d const&) = delete;

	MayaWindow* const Window;

	void UseProjection(float width, float height);
	void UseProjection(MayaFvec2 size);
	void UseWindowProjection();

	void UseCamera(class MayaCamera2d* camera);

	void UseColor(int r, int g, int b, int a = 255);
	void UseColor(MayaIvec3 color);
	void UseColor(MayaIvec4 color);
	void UseColor(unsigned hexcode, bool hasopacity = false);

	void UseTexture(MayaTexture* texture);
	
	void PushScissor(MayaFvec2 pos, MayaFvec2 size);
	void PopScissor();

	void DrawRect(float x, float y, float width, float height);
	void DrawRect(MayaFvec2 pos, MayaFvec2 size);

	void DrawRectBorder(float x, float y, float width, float height, int linewidth = 1);
	void DrawRectBorder(MayaFvec2 pos, MayaFvec2 size, int linewidth = 1);

	void DrawLine(float startx, float starty, float endx, float endy);
	void DrawLine(MayaFvec2 start, MayaFvec2 end);

	void DrawOval(float x, float y, float width, float height, int completion = 100);
	void DrawOval(MayaFvec2 pos, MayaFvec2 size, int completion = 100);

	void DrawIsoTriangle(float x, float y, float width, float height);
	void DrawIsoTriangle(MayaFvec2 pos, MayaFvec2 size);

	void DrawText(MayaFont& font, MayaStringCR text, float x, float y, MayaCorner align = MayaCornerBC);
	void DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos, MayaCorner align = MayaCornerBC);
	void DrawText(class MayaTextDisplay2d& text);
	void DrawText(class MayaTextDisplay2d& text, int start, int end);

	void DrawTick(float x, float y, float width, float height);
	void DrawTick(MayaFvec2 pos, MayaFvec2 size);

private:

	MayaShaderProgramUptr program;
	MayaVertexArrayUptr vao[4];
	MayaBlending blend;
	MayaArrayList<MayaScissorTest> scissors;
	MayaRenderer r;

	void DrawShape(int index);
	void DrawShape(int index, MayaFvec2 pos, MayaFvec2 size);

	// cache
	MayaFvec2 projection;
	MayaCamera2d* camera;
	MayaIvec4 color;
	MayaTexture* texture;
};