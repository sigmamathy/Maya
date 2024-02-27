#include <maya/gui/panel.hpp>
#include <maya/color.hpp>

static constexpr float s_scroll_bar_width = 20.0f;
static constexpr float s_title_height = 40.0f;

MayaPanelGui::MayaPanelGui(MayaGraphicsGui& gui)
	: MayaContainerGui(gui)
{
	size = { 800, 600 };
}

void MayaPanelGui::ReactEvent(MayaEvent& e)
{
	// nothing here :)
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
	cursor_clicked_on_title(false), cursor_prev_pos(0), minimized(0),
	scroll(0), scrollbar(0), scroll_enabled(0), content_size(0)
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
				if (scroll_enabled) {
					scrollbar->SetVisible(!minimized);
				}
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
			if (PointInArea(cp, MayaFvec2(0, size.y / 2 - s_title_height / 2),
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
	else if (auto* me = MayaEventCast<MayaMouseMovedEvent>(e))
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
	bool ac = IsAccessible();

	if (background_visible)
	{
		if (!minimized) {
			g2d.UseColor(ac ? colors.Bg3 : MayaDarkGray);
			g2d.DrawRect(epos, size);
		}
		g2d.UseColor(ac ? colors.Bg2 : MayaGray);
		g2d.DrawRect(epos + MayaFvec2(0, size.y / 2 - s_title_height / 2), MayaFvec2(size.x, s_title_height));
	}

	g2d.UseColor(ac ? colors.Fg1 : MayaWhite);
	title.SetPosition(epos + MayaFvec2(-size.x / 2 + 50, size.y / 2 - s_title_height / 2));
	g2d.DrawText(title);
	expand->Draw(g2d);
	if (scrollbar && !minimized)
		scrollbar->Draw(g2d);

	if (!minimized)
	{
		MayaFvec2 cvp, cvs;
		GetContentView(cvp, cvs);
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

void MayaTitlePanelGui::SetEnableScroll(bool enable)
{
	if (enable && !scrollbar)
	{
		scrollbar = &gui->CreateScrollBar();
		scrollbar->SetParent(this, false);
		scrollbar->SetBackgroundVisible(false);
		scrollbar->SetPosition((size.x - s_scroll_bar_width) / 2, -s_title_height / 2 + s_scroll_bar_width / 2);
		scrollbar->SetSize(s_scroll_bar_width, size.y - s_title_height - s_scroll_bar_width);
		scrollbar->SetScrollMax(size.y);
		scrollbar->SetScrollView(size.y);
		scrollbar->SetEventCallback([&](MayaEventGui& e) {
			if (e.Type == e.Interact)
				scroll.y = -scrollbar->GetValue();
			});
	}

	scroll_enabled = enable;
	if (scrollbar)
		scrollbar->SetVisible(enable && !minimized);
}

bool MayaTitlePanelGui::IsScrollEnabled() const
{
	return scroll_enabled;
}

void MayaTitlePanelGui::GetContentView(MayaFvec2& pos, MayaFvec2& size) const
{
	if (!minimized)
	{
		pos = { 0, -s_title_height / 2 };
		size = this->size;
		size.y -= s_title_height;

		if (scroll_enabled)
		{
			size.x -= s_scroll_bar_width;
			pos.x -= s_scroll_bar_width / 2;
		}
	}
	else
	{
		pos = { 0, 0 };
		size = { 0, 0 };
	}
}

void MayaTitlePanelGui::SetSize(MayaFvec2 size)
{
	MayaPanelGui::SetSize(size);
	expand->SetPosition(6 - size.x / 2 + s_title_height / 2, size.y / 2 - s_title_height / 2);
	if (scrollbar) {
		scrollbar->SetPosition((size.x - s_scroll_bar_width) / 2, -s_title_height / 2 + s_scroll_bar_width / 2);
		scrollbar->SetSize(s_scroll_bar_width, size.y - s_title_height - s_scroll_bar_width);
		scrollbar->SetScrollView(size.y);
	}
}

void MayaTitlePanelGui::SetContentSize(MayaFvec2 size)
{
	content_size = size;
	scrollbar->SetScrollMax(size.y);
}

MayaFvec2 MayaTitlePanelGui::GetContentSize() const
{
	return content_size;
}

MayaFvec2 MayaTitlePanelGui::GetContentShift() const
{
	return -scroll;
}