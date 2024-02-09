#include <maya/gui/textfield.hpp>
#include <maya/color.hpp>
#include <glad/glad.h>

MayaGraphicsGUI::TextField::TextField(MayaGraphicsGUI& gui)
	: Component(gui), text(gui.GetDefaultFont(), "Enter Text"),
	careti(-1), caretpos(0), caret_timer(MayaGetCurrentTimeSinceInit())
{
	size = { 200, 60 };
	color0 = { 65, 71, 74, 255 };
	color1 = { 109, 123, 130, 255 };
	text.SetTextAlign(MayaCornerBL);
}

void MayaGraphicsGUI::TextField::Draw(MayaGraphics2D& g2d)
{
	if (!visible)
		return;
	g2d.UseColor(enabled ? color0 : MayaGray);
	g2d.DrawRect(position + GetRelativePosition(), size);

	auto tpos = MayaFvec2(position.x - size.x * 0.5f + 5,
		position.y - text.GetFont().GetMaxHeight() * 0.5f) + GetRelativePosition();
	g2d.UseColor(MayaWhite);
	text.SetPosition(tpos);

	glEnable(GL_SCISSOR_TEST);
	MayaIvec2 sc = position + GetRelativePosition() + gui->Window->GetSize() * 0.5f - size * 0.5f;
	glScissor(sc.x, sc.y, static_cast<int>(size.x - 4), static_cast<int>(size.y));
	g2d.DrawText(text);
	glDisable(GL_SCISSOR_TEST);

	if (enabled && careti != -1)
	{
		float crnt = (float) MayaGetCurrentTimeSinceInit();
		if (crnt - caret_timer > 1.0f)
			caret_timer += 1.0f;
		if (crnt - caret_timer <= 0.5f)
			g2d.DrawLine(tpos.x + caretpos, position.y + GetRelativePosition().y + size.y * 0.5f - 10,
				tpos.x + caretpos, position.y + GetRelativePosition().y - size.y * 0.5f + 10);

		g2d.UseColor(color1);
		g2d.DrawRectBorder(position + GetRelativePosition(), size);
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
			}
			else if (ke->KeyCode == MayaKeyRight && careti != text.GetLength())
			{
				char c = text.GetString()[careti];
				careti++;
				caretpos += text.GetFont()[c].Advance;
				caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
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

	for (int i = 0, advance = 0; i < text.GetLength(); i++)
	{
		caretpos = advance;
		advance += text.GetFont()[text.GetString()[i]].Advance;
		if (advance > d) {
			careti = i;
			break;
		}
		if (i == text.GetLength() - 1)
		{
			caretpos = advance;
			careti = i + 1;
		}
	}

	caret_timer = static_cast<float>(MayaGetCurrentTimeSinceInit());
}