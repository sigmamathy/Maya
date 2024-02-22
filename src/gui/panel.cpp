#include <maya/gui/panel.hpp>

MayaPanelGui::MayaPanelGui(MayaGraphicsGui& gui)
	: MayaContainerGui(gui)
{
	size = { 800, 600 };
}

void MayaPanelGui::ReactEvent(MayaEvent& e)
{

}

void MayaPanelGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	g2d.UseColor(colors[1]);
	g2d.DrawRect(epos, size);

	g2d.PushScissor(epos, size);
	for (int i = 0; i < childs.size(); i++)
		childs[i]->Draw(g2d);
	g2d.PopScissor();
}