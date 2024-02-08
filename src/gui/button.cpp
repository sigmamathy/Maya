#include <maya/gui/button.hpp>
#include <maya/transformation.hpp>
#include <maya/color.hpp>

MayaGraphicsGUI::Button::Button(MayaGraphicsGUI& gui)
	: Component(gui), pressed(false)
{
	position = { 0, 0 };
	size = { 200, 100 };
	color0 = { 89, 97, 107, 255 };
	color1 = { 117, 129, 143, 255 };
	text = "Button";
}

void MayaGraphicsGUI::Button::Draw(MayaGraphics2D& g2d)
{
	g2d.UseColor(IsButtonPressed() ? color1 : color0);
	g2d.DrawRect(position + GetRelativePosition(), size);

	g2d.UseColor(MayaWhite);
	g2d.DrawText(0, text, position + GetRelativePosition(), MayaTextAlignCC);
}

void MayaGraphicsGUI::Button::ReactEvent(MayaEvent& e)
{
	if (e.GetEventID() == MayaMouseEvent::EventID)
	{
		auto* me = static_cast<MayaMouseEvent*>(&e);
		if (me->Down && me->Button == MayaMouseButtonLeft && IsButtonTouched())
		{
			pressed = true;
		}
		if (pressed && !me->Down && me->Button == MayaMouseButtonLeft)
		{
			pressed = false;
			if (callback)
				callback();
		}
	}
}

void MayaGraphicsGUI::Button::SetEventCallback(MayaFunctionCR<void()> callback)
{
	this->callback = callback;
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