#include <maya/gui/label.hpp>
#include <maya/color.hpp>

MayaLabelGui::MayaLabelGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), text(gui.GetDefaultFont(), "")
{
	text.SetTextAlign(MayaCornerCC);
}

void MayaLabelGui::Draw(MayaGraphics2d& g2d)
{
	text.SetPosition(position + GetRelativePosition());
	g2d.UseColor(colors[3]);
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