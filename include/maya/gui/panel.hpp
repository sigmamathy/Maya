#pragma once

#include "./graphics.hpp"
#include "./button.hpp"
#include "./scrollbar.hpp"
#include "../2d/textdisplay.hpp"

class MayaPanelGui : public MayaContainerGui
{
public:

	MayaPanelGui(MayaGraphicsGui& gui);

	void ReactEvent(MayaEvent& e) override;
	void Draw(MayaGraphics2d& g2d) override;
};

class MayaTitlePanelGui : public MayaPanelGui
{
public:

	MayaTitlePanelGui(MayaGraphicsGui& gui);

	void ReactEvent(MayaEvent& e) override;
	void Draw(MayaGraphics2d& g2d) override;

	void SetTitle(MayaStringCR title);
	MayaStringCR GetTitle() const;

	void SetEnableScroll(bool enable);
	bool IsScrollEnabled() const;
	void SetContentSize(MayaFvec2 size);
	MayaFvec2 GetContentSize() const;

	void GetContentView(MayaFvec2& pos, MayaFvec2& size) const override;
	void SetSize(MayaFvec2 size) override;
	MayaFvec2 GetContentShift() const override;

private:

	MayaTextDisplay2d title;
	bool cursor_clicked_on_title;
	MayaFvec2 cursor_prev_pos;

	MayaButtonGui* expand;
	bool minimized;

	MayaScrollBarGui* scrollbar;
	bool scroll_enabled;
	MayaFvec2 scroll;
	MayaFvec2 content_size;
};