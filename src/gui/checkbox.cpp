#include <maya/gui/checkbox.hpp>
#include <maya/color.hpp>

MayaGraphicsGUI::Checkbox::Checkbox(MayaGraphicsGUI& gui)
	: Component(gui), text(gui.GetDefaultFont(), "Checkbox :o"), selected(false)
{
	size = { 30, 30 };
	color0 = { 65, 71, 74, 255 };
	color1 = { 109, 123, 130, 255 };
	text.SetTextAlign(MayaCornerCL);
}

void MayaGraphicsGUI::Checkbox::Draw(MayaGraphics2D& g2d)
{
	auto epos = position + GetRelativePosition();
	g2d.UseColor(color0);
	g2d.DrawRect(epos, size);
	
	g2d.UseColor(color1);
	g2d.DrawRectBorder(epos, size, 2);

	g2d.UseColor(MayaWhite);

	if (selected)
	{
		g2d.UseTexture(&gui->GetDefaultTickSymbol());
		g2d.DrawRect(epos, size * 0.8f);
		g2d.UseTexture(nullptr);
	}

	text.SetPosition(epos + MayaFvec2(size.x, 0));
	g2d.DrawText(text);
}

void MayaGraphicsGUI::Checkbox::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down && me->Button == MayaMouseButton1 && IsCheckboxTouched())
		{
			selected = !selected;
		}
	}
}

void MayaGraphicsGUI::Checkbox::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaGraphicsGUI::Checkbox::GetText() const
{
	return text.GetString();
}

void MayaGraphicsGUI::Checkbox::SetSelected(bool select)
{
	selected = select;
}

bool MayaGraphicsGUI::Checkbox::IsSelected() const
{
	return selected;
}

bool MayaGraphicsGUI::Checkbox::IsCheckboxTouched() const
{
	auto* window = gui->Window;
	MayaFvec2 cp = window->GetCursorPosition();
	cp = cp - window->GetSize() / 2;
	cp.y = -cp.y;
	auto pos = position + GetRelativePosition();
	return cp.x >= pos.x - size.x * 0.5f && cp.x <= pos.x + size.x * 0.5f
		&& cp.y >= pos.y - size.y * 0.5f && cp.y <= pos.y + size.y * 0.5f;
}