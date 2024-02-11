#include <maya/gui/textfield.hpp>
#include <maya/color.hpp>
#include <maya/deviceinfo.hpp>

MayaGraphicsGUI::TextField::TextField(MayaGraphicsGUI& gui)
	: Component(gui), text(gui.GetDefaultFont(), ""), description(gui.GetDefaultFont(), "Enter Text"),
	careti(-1), caretpos(0), caret_timer(static_cast<float>(MayaGetCurrentTimeSinceInit())), scroll(0)
{
	size = { 200, 60 };
	color0 = { 65, 71, 74, 255 };
	color1 = { 109, 123, 130, 255 };
}

void MayaGraphicsGUI::TextField::Draw(MayaGraphics2D& g2d)
{
	if (!visible)
		return;
	g2d.UseColor(enabled ? color0 : MayaGray);
	g2d.DrawRect(position + GetRelativePosition(), size);

	auto tpos = MayaFvec2(position.x - size.x * 0.5f + 5,
		position.y - text.GetFont().GetMaxHeight() * 0.5f) + GetRelativePosition();
	tpos.x -= scroll;

	if (text.GetLength() != 0)
	{
		g2d.UseColor(MayaWhite);
		text.SetPosition(tpos);
		g2d.BeginScissor(position + GetRelativePosition(), MayaFvec2(size.x - 10, size.y));
		g2d.DrawText(text);
		g2d.EndScissor();
	}
	else
	{
		g2d.UseColor(MayaGray);
		description.SetPosition(tpos);
		g2d.BeginScissor(position + GetRelativePosition(), MayaFvec2(size.x - 10, size.y));
		g2d.DrawText(description);
		g2d.EndScissor();
	}
	

	if (enabled && careti != -1)
	{

		g2d.UseColor(MayaWhite);
		float crnt = (float) MayaGetCurrentTimeSinceInit();
		if (crnt - caret_timer > 1.0f)
			caret_timer += 1.0f;
		if (crnt - caret_timer <= 0.5f)
			g2d.DrawLine(tpos.x + caretpos, position.y + GetRelativePosition().y + size.y * 0.5f - 10,
				tpos.x + caretpos, position.y + GetRelativePosition().y - size.y * 0.5f + 10);

		g2d.UseColor(color1);
		g2d.DrawRectBorder(position + GetRelativePosition(), size, 1);
	}
}

void MayaGraphicsGUI::TextField::ReactEvent(MayaEvent& e)
{
	if (!visible || !enabled)
		return;

	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down && me->Button == MayaMouseButton1)
		{
			UpdateCaretPos();
		}
	}

	else if (auto* ce = MayaEventCast<MayaCharEvent>(e))
	{
		if (careti != -1) {
			text.InsertCharAt(careti, ce->Char);
			careti++;
			caretpos += text.GetFont()[ce->Char].Advance;
			if (caretpos - scroll > size.x - 10)
				scroll = caretpos - static_cast<int>(size.x) + 10;
		}
	}

	else if (auto* ke = MayaEventCast<MayaKeyEvent>(e))
	{
		if (careti != -1 && ke->Down)
		{
			if (ke->KeyCode == MayaKeyBackspace && careti != 0)
			{
				char c = text.RemoveCharAt(careti - 1);
				careti--;
				caretpos -= text.GetFont()[c].Advance;
				caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
				if (caretpos < scroll)
					scroll = caretpos;
			}
			else if (ke->KeyCode == MayaKeyDelete && careti != text.GetLength())
			{
				char c = text.RemoveCharAt(careti);
				caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
			}
			else if (ke->KeyCode == MayaKeyLeft && careti != 0)
			{
				char c = text.GetString()[careti - 1];
				careti--;
				caretpos -= text.GetFont()[c].Advance;
				caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
				if (caretpos < scroll)
					scroll = caretpos;
			}
			else if (ke->KeyCode == MayaKeyRight && careti != text.GetLength())
			{
				char c = text.GetString()[careti];
				careti++;
				caretpos += text.GetFont()[c].Advance;
				caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
				if (caretpos - scroll > size.x - 10)
					scroll = caretpos - static_cast<int>(size.x) + 10;
			}
			else if (ke->KeyCode == MayaKeyV && (ke->Mods & MayaModControl))
			{
				MayaString cs = MayaGetClipBoardString();
				MayaString s;
				s.reserve(cs.length());
				for (char c : cs)
					if (c >= 32 && c <= 127)
						s += c;
				if (!s.empty()) {
					text.InsertStringAt(careti, s);
					int adv = 0;
					for (char c : s)
						adv += text.GetFont()[c].Advance;
					careti += static_cast<int>(s.length());
					caretpos += adv;
					caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
					if (caretpos - scroll > size.x - 10)
						scroll = caretpos - static_cast<int>(size.x) + 10;
				}
			}
		}
	}
}

bool MayaGraphicsGUI::TextField::IsTextFieldTouched() const
{
	auto* window = gui->Window;
	MayaFvec2 cp = window->GetCursorPosition();
	cp = cp - window->GetSize() / 2;
	cp.y = -cp.y;
	auto pos = position + GetRelativePosition();
	return cp.x >= pos.x - size.x * 0.5f && cp.x <= pos.x + size.x * 0.5f
		&& cp.y >= pos.y - size.y * 0.5f && cp.y <= pos.y + size.y * 0.5f;
}

void MayaGraphicsGUI::TextField::UpdateCaretPos()
{
	if (!IsTextFieldTouched()) {
		careti = -1;
		return;
	}

	MayaFvec2 cp = gui->Window->GetCursorPosition();
	cp = cp - gui->Window->GetSize() / 2;
	float d = cp.x - (position.x + GetRelativePosition().x - size.x * 0.5f);
	caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());

	if (text.GetLength() == 0)
	{
		careti = 0;
		caretpos = 0;
	}

	for (int i = 0, advance = 0; i < static_cast<int>(text.GetLength()); i++)
	{
		caretpos = advance;
		advance += text.GetFont()[text.GetString()[i]].Advance;
		if (advance - scroll > d) {
			careti = i;
			break;
		}
		if (i == text.GetLength() - 1)
		{
			caretpos = advance;
			careti = i + 1;
		}
	}
}

void MayaGraphicsGUI::TextField::SetFont(MayaFont* font)
{
	if (!font)
		font = &gui->GetDefaultFont();
	text.SetFont(*font);
	description.SetFont(*font);
}

void MayaGraphicsGUI::TextField::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaGraphicsGUI::TextField::GetText() const
{
	return text.GetString();
}

void MayaGraphicsGUI::TextField::SetDescription(MayaStringCR desc)
{
	description = desc;
}

MayaStringCR MayaGraphicsGUI::TextField::GetDescription() const
{
	return description.GetString();
}