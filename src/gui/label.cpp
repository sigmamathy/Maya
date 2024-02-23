#include <maya/gui/label.hpp>
#include <maya/color.hpp>

MayaLabelGui::MayaLabelGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), text(gui.GetDefaultFont(), "")
{
	text.SetTextAlign(MayaCornerCC);
}

void MayaLabelGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	if (background_visible) {
		g2d.UseColor(colors.Bg2);
		g2d.DrawRect(epos, size);
	}

	text.SetPosition(epos);
	g2d.UseColor(colors.Fg1);
	g2d.DrawText(text);
}

void MayaLabelGui::ReactEvent(MayaEvent& e)
{

}

void MayaLabelGui::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaLabelGui::GetText() const
{
	return text.GetString();
}