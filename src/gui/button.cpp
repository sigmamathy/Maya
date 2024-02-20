#include <maya/gui/button.hpp>
#include <maya/transformation.hpp>
#include <maya/color.hpp>

MayaButtonGui::MayaButtonGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), pressed(false), text(gui.GetDefaultFont(), "Button")
{
	position = { 0, 0 };
	size = { 160, 80 };
	text.SetTextAlign(MayaCornerCC);
}

void MayaButtonGui::Draw(MayaGraphics2d& g2d)
{
	if (!visible)
		return;

	auto epos = GetExactPosition();

	g2d.UseColor(enabled ? (IsButtonPressed() ? colors[1] : colors[0]) : MayaGray);
	g2d.DrawRect(epos, size);

	g2d.UseColor(colors[3]);
	text.SetPosition(epos);
	g2d.DrawText(text);

	if (enabled && CursorInArea())
	{
		g2d.UseColor(colors[2]);
		g2d.DrawRectBorder(epos, size, 3);
	}
}

void MayaButtonGui::ReactEvent(MayaEvent& e)
{
	if (!visible || !enabled)
		return;

	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down && me->Button == MayaMouseButtonLeft && CursorInArea())
		{
			pressed = true;
		}
		if (pressed && !me->Down && me->Button == MayaMouseButtonLeft)
		{
			pressed = false;
			SendCallback(MayaEventGui::Interact);
		}
	}
}

void MayaButtonGui::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaButtonGui::GetText() const
{
	return text.GetString();
}

bool MayaButtonGui::IsButtonPressed() const
{
	return pressed;
}