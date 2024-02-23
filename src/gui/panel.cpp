#include <maya/gui/panel.hpp>

static constexpr float s_title_height = 40.0f;

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

	if (background_visible) {
		g2d.UseColor(colors.Bg3);
		g2d.DrawRect(epos, size);
	}

	g2d.PushScissor(epos, size);
	for (int i = 0; i < childs.size(); i++)
		childs[i]->Draw(g2d);
	g2d.PopScissor();
}

MayaTitlePanelGui::MayaTitlePanelGui(MayaGraphicsGui& gui)
	: MayaPanelGui(gui), title(gui.GetDefaultFont(), "Window"),
	cursor_clicked_on_title(false), cursor_prev_pos(0), minimized(0)
{
	title.SetTextAlign(MayaCornerCL);
	expand = &gui.CreateButton();
	expand->SetParent(this, 0);
	expand->SetBackgroundVisible(false);
	expand->SetButtonIcon(expand->TriangleDown);
	expand->SetPosition(6 - size.x / 2 + s_title_height / 2, size.y / 2 - s_title_height / 2);
	expand->SetSize(s_title_height * 0.5f, s_title_height* 0.3f);
	expand->SetEventCallback(
		[&](MayaEventGui e) -> void {
			if (e.Type == e.Interact) {
				minimized = !minimized;
				expand->SetButtonIcon(minimized ? expand->TriangleUp : expand->TriangleDown);
			}
		});
}

void MayaTitlePanelGui::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down)
		{
			auto cp = e.Window->GetCursorPosition() - e.Window->GetSize() / 2;
			cp.y *= -1;
			if (PointInArea(cp, position + MayaFvec2(0, size.y / 2 - s_title_height / 2),
				MayaFvec2(size.x, s_title_height)))
			{
				cursor_prev_pos = cp;
				cursor_clicked_on_title = true;
			}
		}
		else
		{
			cursor_clicked_on_title = false;
		}
	}
	if (auto* me = MayaEventCast<MayaMouseMovedEvent>(e))
	{
		if (cursor_clicked_on_title)
		{
			auto cp = e.Window->GetCursorPosition() - e.Window->GetSize() / 2;
			cp.y *= -1;
			auto d = cp - cursor_prev_pos;
			position = d + position;
			cursor_prev_pos = cp;
		}
	}
}

void MayaTitlePanelGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	if (background_visible)
	{
		if (!minimized) {
			g2d.UseColor(colors.Bg3);
			g2d.DrawRect(epos, size);
		}
		g2d.UseColor(colors.Bg2);
		g2d.DrawRect(epos + MayaFvec2(0, size.y / 2 - s_title_height / 2), MayaFvec2(size.x, s_title_height));
	}

	g2d.UseColor(colors.Fg1);
	title.SetPosition(epos + MayaFvec2(-size.x / 2 + 50, size.y / 2 - s_title_height / 2));
	g2d.DrawText(title);
	expand->Draw(g2d);

	if (!minimized)
	{
		MayaFvec2 cvp, cvs;
		GetContainerView(cvp, cvs);
		g2d.PushScissor(epos + cvp, cvs);
		for (int i = 0; i < childs.size(); i++)
			childs[i]->Draw(g2d);
		g2d.PopScissor();
	}
}

void MayaTitlePanelGui::SetTitle(MayaStringCR title)
{
	this->title = title;
}

MayaStringCR MayaTitlePanelGui::GetTitle() const
{
	return title.GetString();
}

void MayaTitlePanelGui::GetContainerView(MayaFvec2& pos, MayaFvec2& size) const
{
	if (!minimized)
	{
		pos = { 0, -s_title_height / 2 };
		size = this->size;
		size.y -= s_title_height;
	}
	else
	{
		pos = { 0, 0 };
		size = { 0, 0 };
	}
}

void MayaTitlePanelGui::SetSize(MayaFvec2 size)
{
	MayaComponentGui::SetSize(size);
	expand->SetPosition(6 - size.x / 2 + s_title_height / 2, size.y / 2 - s_title_height / 2);
}