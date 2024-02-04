#pragma once

#include "../core.hpp"
#include "../math.hpp"
#include "../renderer.hpp"
#include "../window.hpp"
#include "../font.hpp"

class MayaGraphics2D
{
public:

	enum TextAlign {
		TextAlignLeft = 0b1, TextAlignCenterH = 0b10, TextAlignRight = 0b11,
		TextAlignBottom = 0b100, TextAlignCenterV = 0b1000, TextAlignTop = 0b1100,
		TextAlignTL = TextAlignTop | TextAlignLeft,
		TextAlignTC = TextAlignTop | TextAlignCenterH,
		TextAlignTR = TextAlignTop | TextAlignRight,
		TextAlignCL = TextAlignCenterV | TextAlignLeft,
		TextAlignCC = TextAlignCenterV | TextAlignCenterH,
		TextAlignCR = TextAlignCenterV | TextAlignRight,
		TextAlignBL = TextAlignBottom | TextAlignLeft,
		TextAlignBC = TextAlignBottom | TextAlignCenterH,
		TextAlignBR = TextAlignBottom | TextAlignRight,
	};

	class Camera;
	class Renderer;
	class TextDisplay;

public:

	MayaWindow* const Window;

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

	void DrawOval(float x, float y, float width, float height);

	void DrawOval(MayaFvec2 pos, MayaFvec2 size);

	void DrawText(MayaFont& font, MayaStringCR text, float x, float y);

	void DrawText(MayaFont& font, MayaStringCR text, MayaFvec2 pos = MayaFvec2(0));

	void DrawText(TextDisplay& text);

private:

	MayaGraphics2D(MayaWindow* window);

	MayaGraphics2D(MayaGraphics2D const&) = delete;

	MayaGraphics2D& operator=(MayaGraphics2D const&) = delete;

	friend class MayaWindow;

private:

	MayaShaderProgramUptr program;
	MayaVertexArrayUptr squarevao, circlevao64;
	Camera* camera;
	MayaTexture* texture;

};