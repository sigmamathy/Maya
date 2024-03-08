#include <maya/gui/textfield.hpp>
#include <maya/color.hpp>
#include <maya/deviceinfo.hpp>

MayaTextFieldGui::MayaTextFieldGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), text(gui.GetDefaultFont(), ""), description(gui.GetDefaultFont(), "Enter Text"),
	careti(-1), caretpos(0), caret_timer(MayaGetLibraryManager()->GetTimeSince()), scroll(0)
{
	size = { 300, 60 };
}

void MayaTextFieldGui::Draw(MayaGraphics2d& g2d)
{
	if (!visible)
		return;
	auto epos = GetExactPosition();
	auto ac = IsAccessible();

	if (background_visible) {
		g2d.UseColor(ac ? colors.Bg2 : MayaGray);
		g2d.DrawRect(epos, size);
	}

	auto tpos = epos + MayaFvec2(-size.x * 0.5f + 5 - scroll, -text.GetFont().GetMaxHeight() * 0.5f);

	g2d.PushScissor(epos, MayaFvec2(size.x - 10, size.y));

	if (text.GetLength() != 0)
	{
		g2d.UseColor(ac ? colors.Fg1 : MayaWhite);
		text.SetPosition(tpos);
		g2d.DrawText(text);
	}
	else
	{
		g2d.UseColor(ac ? colors.Fg2 : MayaLightGray);
		description.SetPosition(tpos);
		g2d.DrawText(description);
	}

	g2d.PopScissor();

	if (ac && careti != -1)
	{
		g2d.UseColor(colors.Fg1);
		float crnt = MayaGetLibraryManager()->GetTimeSince();
		if (crnt - caret_timer > 1.0f)
			caret_timer += 1.0f;
		if (crnt - caret_timer <= 0.5f)
			g2d.DrawLine(tpos.x + caretpos, epos.y + size.y * 0.5f - 10,
				tpos.x + caretpos, epos.y - size.y * 0.5f + 10);

		g2d.UseColor(colors.Border);
		g2d.DrawRectBorder(epos, size, 1);
	}
}

void MayaTextFieldGui::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Down && me->Button == MayaMouseButton1)
		{
			SetCaretPosToMousePos();
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
			SendCallback(MayaEventGui::Typing);
		}
	}

	else if (auto* ke = MayaEventCast<MayaKeyEvent>(e))
	{
		if (careti != -1 && ke->Down)
		{
			if (ke->Keycode == MayaKeyBackspace && careti != 0)
			{
				char c = text.RemoveCharAt(careti - 1);
				careti--;
				caretpos -= text.GetFont()[c].Advance;
				caret_timer = MayaGetLibraryManager()->GetTimeSince();
				if (caretpos < scroll)
					scroll = caretpos;
				SendCallback(MayaEventGui::Typing);
			}
			else if (ke->Keycode == MayaKeyDelete && careti != text.GetLength())
			{
				char c = text.RemoveCharAt(careti);
				caret_timer = MayaGetLibraryManager()->GetTimeSince();
				SendCallback(MayaEventGui::Typing);
			}
			else if (ke->Keycode == MayaKeyLeft && careti != 0)
			{
				char c = text.GetString()[careti - 1];
				careti--;
				caretpos -= text.GetFont()[c].Advance;
				caret_timer = MayaGetLibraryManager()->GetTimeSince();
				if (caretpos < scroll)
					scroll = caretpos;
			}
			else if (ke->Keycode == MayaKeyRight && careti != text.GetLength())
			{
				char c = text.GetString()[careti];
				careti++;
				caretpos += text.GetFont()[c].Advance;
				caret_timer = MayaGetLibraryManager()->GetTimeSince();
				if (caretpos - scroll > size.x - 10)
					scroll = caretpos - static_cast<int>(size.x) + 10;
			}
			else if (ke->Keycode == MayaKeyV && (ke->Mods & MayaModControl))
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
					caret_timer = MayaGetLibraryManager()->GetTimeSince();
					if (caretpos - scroll > size.x - 10)
						scroll = caretpos - static_cast<int>(size.x) + 10;
					SendCallback(MayaEventGui::Typing);
				}
			}
			else if (ke->Keycode == MayaKeyEnter)
			{
				SendCallback(MayaEventGui::Interact);
			}
		}
	}
}

void MayaTextFieldGui::SetCaretPosToMousePos()
{
	if (!CursorInArea()) {
		careti = -1;
		return;
	}

	MayaFvec2 cp = gui->Window->GetCursorPosition();
	cp = cp - gui->Window->GetSize() / 2;
	float d = cp.x - (GetExactPosition().x - size.x * 0.5f);
	caret_timer = MayaGetLibraryManager()->GetTimeSince();

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

void MayaTextFieldGui::SetFont(Font* font)
{
	if (!font)
		font = &gui->GetDefaultFont();
	text.SetFont(*font);
	description.SetFont(*font);
}

void MayaTextFieldGui::SetText(MayaStringCR text)
{
	this->text = text;
}

MayaStringCR MayaTextFieldGui::GetText() const
{
	return text.GetString();
}

void MayaTextFieldGui::SetDescription(MayaStringCR desc)
{
	description = desc;
}

MayaStringCR MayaTextFieldGui::GetDescription() const
{
	return description.GetString();
}