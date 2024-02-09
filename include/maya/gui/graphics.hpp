#pragma once

#include "../2d/graphics.hpp"

class MayaGraphicsGUI
{
public:

// ----------- Components ----------- //

	class Component;
	class Button;
	class TextField;

// ----------- Events ----------- //

	struct UserEvent;
	struct UserMouseEvent;
	MAYA_TYPEDEF0(UserEventCallback, MayaFunction<void(UserEvent&)>);

public:

	class Component
	{
	public:
		Component(MayaGraphicsGUI& gui);
		virtual ~Component() = default;

		virtual void ReactEvent(MayaEvent& e) = 0;
		virtual void Draw(MayaGraphics2D& g2d) = 0;

		void SetPositionRelativeTo(Component* comp);
		void SetPositionRelativeTo(MayaCorner relpos);
		MayaFvec2 GetRelativePosition() const;

		void SetPosition(float x, float y);
		void SetSize(float width, float height);
		virtual void SetPosition(MayaFvec2 pos);
		virtual void SetSize(MayaFvec2 size);
		virtual MayaFvec2 GetPosition() const;
		virtual MayaFvec2 GetSize() const;

		void SetColor0(MayaIvec4 color);
		void SetColor1(MayaIvec4 color);
		MayaIvec4 GetColor0() const;
		MayaIvec4 GetColor1() const;

		void SetVisible(bool visible);
		void SetEnabled(bool enable);
		bool IsVisible() const;
		bool IsEnabled() const;

		void SetEventCallback(UserEventCallbackCR callback);

	protected:
		MayaGraphicsGUI* gui;
		MayaFvec2 position, size;
		MayaIvec4 color0, color1;
		bool visible, enabled;
		UserEventCallback callback;

		Component* relativeto;
		MayaCorner relwpos;
	};

public:

	MayaGraphicsGUI(MayaWindow& window);
	MayaGraphicsGUI(MayaGraphicsGUI const&) = delete;
	MayaGraphicsGUI& operator=(MayaGraphicsGUI const&) = delete;
	~MayaGraphicsGUI();

	MayaWindow* const Window;

	Button& CreateButton();
	TextField& CreateTextField();

	void Draw();

	MayaFont& GetDefaultFont();

private:

	MayaArrayList<MayaUptr<Component>> components;
	MayaGraphics2D g2d;
	unsigned callbackid;
	MayaFontUptr default_font;
};

struct MayaGraphicsGUI::UserEvent : public MayaEvent
{
	Component* Source;
};

struct MayaGraphicsGUI::UserMouseEvent : public MayaGraphicsGUI::UserEvent
{
	MAYA_DEFINE_EVENT_ID(0x10);
	MayaMouseButton Button;
	bool Down;
};