#include <maya/gui/checkbox.hpp>
#include <maya/color.hpp>

MayaCheckboxGui::MayaCheckboxGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), text(gui.GetDefaultFont(), "Checkbox :o"), selected(false)
{
	size = { 30, 30 };
	text.SetTextAlign(MayaCornerCL);
}

void MayaCheckboxGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	if (background_visible) {
		g2d.UseColor(colors.Bg2);
		g2d.DrawRect(epos, size);
	}
	
	g2d.UseColor(colors.Border);
	g2d.DrawRectBorder(epos, size, 2);

	g2d.UseColor(colors.Fg1);
	
	if (selected)
		g2d.DrawTick(epos, size * 0.8f);

	text.SetPosition(epos + MayaFvec2(size.x, 0));
	g2d.DrawText(text);
}

void MayaCheckboxGui::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down && me->Button == MayaMouseButton1 && CursorInArea())
		{
			selected = !selected;
			SendCallback(MayaEventGui::Interact);
		}
	}
}

void MayaCheckboxGui::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaCheckboxGui::GetText() const
{
	return text.GetString();
}

void MayaCheckboxGui::SetSelected(bool select)
{
	selected = select;
}

bool MayaCheckboxGui::IsSelected() const
{
	return selected;
}