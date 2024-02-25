#include <maya/gui/button.hpp>
#include <maya/transformation.hpp>
#include <maya/color.hpp>

MayaButtonGui::MayaButtonGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), pressed(false), text(gui.GetDefaultFont(), "Button"), icon(NoIcon)
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
	bool ac = IsAccessible();

	if (background_visible) {
		g2d.UseColor(ac ? (IsButtonPressed() ? colors.Bg2 : colors.Bg1) : MayaGray);
		g2d.DrawRect(epos, size);
	}

	g2d.UseColor(ac ? colors.Fg1 : MayaWhite);

	switch (icon)
	{
		case TriangleUp:
			g2d.DrawIsoTriangle(epos, size);
			break;

		case TriangleDown:
			g2d.DrawIsoTriangle(epos, MayaFvec2(size.x, -size.y));
			break;

		case Tick:
			g2d.DrawTick(epos, size);
			break;

		default:
			text.SetPosition(epos);
			g2d.DrawText(text);

			if (enabled && CursorInArea())
			{
				g2d.UseColor(colors.Border);
				g2d.DrawRectBorder(epos, size, 3);
			}
	}
}

void MayaButtonGui::ReactEvent(MayaEvent& e)
{
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

void MayaButtonGui::SetButtonIcon(ButtonIcon icon)
{
	this->icon = icon;
}

MayaButtonGui::ButtonIcon MayaButtonGui::GetButtonIcon() const
{
	return icon;
}

bool MayaButtonGui::IsButtonPressed() const
{
	return pressed;
}