#include <maya/gui/button.hpp>
#include <maya/transformation.hpp>
#include <maya/color.hpp>

MayaGraphicsGUI::Button::Button(MayaGraphicsGUI& gui)
	: Component(gui), pressed(false)
{
	position = { 50, 50 };
	size = { 200, 100 };
	color0 = { 89, 97, 107, 255 };
	color1 = { 117, 129, 143, 255 };
	text = "Button";
}

void MayaGraphicsGUI::Button::Draw()
{
	UseColor(IsButtonPressed() ? color1 : color0);
	DrawRect(position, size);

	UseColor(MayaWhite);
	DrawText(0, text, position + size * 0.5f, MayaTextAlignCC);
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
	return cp.x >= position.x && cp.x <= position.x + size.x
		&& cp.y >= position.y && cp.y <= position.y + size.y;
}

bool MayaGraphicsGUI::Button::IsButtonPressed() const
{
	return pressed;
}