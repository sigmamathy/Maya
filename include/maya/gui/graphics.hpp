#pragma once

#include "../core.hpp"
#include "../window.hpp"
#include "../renderer.hpp"
#include "../font.hpp"

class MayaGraphicsGUI
{
public:

	class Component;
	class Button;

public:
	class Component
	{
	public:
		Component(MayaGraphicsGUI& gui);
		virtual ~Component() = default;

		virtual void ReactEvent(MayaEvent& e) = 0;
		virtual void Draw() = 0;

		void SetPosition(MayaFvec2 pos);
		void SetSize(MayaFvec2 size);
		MayaFvec2 GetPosition() const;
		MayaFvec2 GetSize() const;

		void SetColor0(MayaIvec4 color);
		void SetColor1(MayaIvec4 color);
		MayaIvec4 GetColor0() const;
		MayaIvec4 GetColor1() const;

	protected:
		MayaGraphicsGUI* gui;
		MayaFvec2 position, size;
		MayaIvec4 color0, color1;

	protected:
		void UseColor(MayaIvec4 color);
		void UseTexture(MayaTexture* texture);
		void DrawRect(MayaFvec2 pos, MayaFvec2 size);
		void DrawText(MayaFont* font, MayaStringCR text, MayaFvec2 pos, MayaTextAlign align);
	};

public:

	MayaWindow* Window;

	Button& CreateButton();

	void Draw();

	void UpdateProjection();

private:

	MayaGraphicsGUI(MayaWindow* window);

	MayaGraphicsGUI(MayaGraphicsGUI const&) = delete;

	MayaGraphicsGUI& operator=(MayaGraphicsGUI const&) = delete;

	MayaFont* GetOpenSansRegularFont();

private:

	MayaArrayList<MayaUptr<Component>> components;
	MayaShaderProgramUptr program;
	MayaVertexArrayUptr squarevao;
	MayaTexture* texture;

	MayaFontUptr opensans_font;

	friend class MayaWindow;
	friend class Component;
};