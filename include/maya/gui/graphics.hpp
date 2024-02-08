#pragma once

#include "../2d/graphics.hpp"

class MayaGraphicsGUI
{
public:

	enum WindowRelativePos {
		RelativeToWindowTopLeft,
		RelativeToWindowTopCenter,
		RelativeToWindowTopRight,
		RelativeToWindowCenterLeft,
		RelativeToNone,
		RelativeToWindowCenterRight,
		RelativeToWindowBottomLeft,
		RelativeToWindowBottomCenter,
		RelativeToWindowBottomRight,
	};

	class Component;
	class Button;

public:
	class Component
	{
	public:
		Component(MayaGraphicsGUI& gui);
		virtual ~Component() = default;

		virtual void ReactEvent(MayaEvent& e) = 0;
		virtual void Draw(MayaGraphics2D& g2d) = 0;

		void SetPositionRelativeTo(Component* comp);
		void SetPositionRelativeTo(WindowRelativePos relpos);
		MayaFvec2 GetRelativePosition() const;

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

		Component* relativeto;
		WindowRelativePos relwpos;
	};

public:

	MayaGraphicsGUI(MayaWindow& window);

	MayaGraphicsGUI(MayaGraphicsGUI const&) = delete;

	MayaGraphicsGUI& operator=(MayaGraphicsGUI const&) = delete;

	MayaWindow* const Window;

	Button& CreateButton();

	void Draw();

private:

	MayaArrayList<MayaUptr<Component>> components;
	MayaGraphics2D g2d;
};