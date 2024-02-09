#include <maya/gui/button.hpp>
#include <maya/transformation.hpp>
#include <maya/color.hpp>

MayaGraphicsGUI::Button::Button(MayaGraphicsGUI& gui)
	: Component(gui), pressed(false)
{
	position = { 0, 0 };
	size = { 160, 80 };
	color0 = { 90, 101, 107, 255 };
	color1 = { 117, 129, 143, 255 };
	text = "Button";
}

void MayaGraphicsGUI::Button::Draw(MayaGraphics2D& g2d)
{
	if (!visible)
		return;

	g2d.UseColor(enabled ? (IsButtonPressed() ? color1 : color0) : MayaGray);
	g2d.DrawRect(position + GetRelativePosition(), size);

	g2d.UseColor(MayaWhite);
	g2d.DrawText(gui->GetDefaultFont(), text, position + GetRelativePosition(), MayaCornerCC);

	if (IsButtonTouched())
	{
		g2d.UseColor(color1);
		g2d.DrawRectBorder(position + GetRelativePosition(), size);
	}
}

void MayaGraphicsGUI::Button::ReactEvent(MayaEvent& e)
{
	if (visible && enabled && e.GetEventID() == MayaMouseEvent::EventID)
	{
		auto* me = static_cast<MayaMouseEvent*>(&e);
		if (me->Down && me->Button == MayaMouseButtonLeft && IsButtonTouched())
		{
			pressed = true;
			if (callback)
			{
				UserMouseEvent ue;
				ue.Window = gui->Window;
				ue.Source = this;
				ue.Button = MayaMouseButtonLeft;
				ue.Down = true;
				callback(ue);
			}
		}
		if (pressed && !me->Down && me->Button == MayaMouseButtonLeft)
		{
			pressed = false;
			if (callback)
			{
				UserMouseEvent ue;
				ue.Window = gui->Window;
				ue.Source = this;
				ue.Button = MayaMouseButtonLeft;
				ue.Down = false;
				callback(ue);
			}
		}
	}
}

void MayaGraphicsGUI::Button::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaGraphicsGUI::Button::GetText() const
{
	return text;
}

bool MayaGraphicsGUI::Button::IsButtonTouched() const
{
	auto* window = gui->Window;
	MayaFvec2 cp = window->GetCursorPosition();
	cp = cp - window->GetSize() / 2;
	cp.y = -cp.y;
	auto pos = position + GetRelativePosition();
	return cp.x >= pos.x - size.x * 0.5f && cp.x <= pos.x + size.x * 0.5f
		&& cp.y >= pos.y - size.y * 0.5f && cp.y <= pos.y + size.y * 0.5f;
}

bool MayaGraphicsGUI::Button::IsButtonPressed() const
{
	return pressed;
}