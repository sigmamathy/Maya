#include <maya/gui/scrollbar.hpp>

MayaScrollBarGui::MayaScrollBarGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), direction(Vertical), max(2), value(0), view(1),
	cursor_clicked(0), cursor_prev_pos(0), prev_value(0)
{
	size = { 20, 300 };
}

void MayaScrollBarGui::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Button == MayaMouseButtonLeft)
		{
			if (me->Down)
			{
				auto cp = e.Window->GetCursorPosition() - e.Window->GetSize() / 2;
				cp.y *= -1;

				MayaFvec2 b0p, b0s;
				ButtonPosAndSize(0, b0p, b0s);
				if (PointInArea(cp, b0p, b0s))
				{
					value -= view * 0.1f;
					if (value < 0) value = 0;
					SendCallback(MayaEventGui::Interact);
					return;
				}

				MayaFvec2 b1p, b1s;
				ButtonPosAndSize(1, b1p, b1s);
				if (PointInArea(cp, b1p, b1s))
				{
					value += view * 0.1f;
					if (value > max - view) value = max - view;
					SendCallback(MayaEventGui::Interact);
					return;
				}
				
				MayaFvec2 scp, scs;
				SlidePosAndSize(scp, scs);

				if (PointInArea(cp, scp, scs))
				{
					cursor_prev_pos = cp;
					prev_value = value;
					cursor_clicked = true;
				}
			}
			else
			{
				cursor_clicked = false;
			}
		}
	}
	else if (auto* me = MayaEventCast<MayaMouseMovedEvent>(e))
	{
		if (cursor_clicked)
		{
			auto cp = e.Window->GetCursorPosition() - e.Window->GetSize() / 2;
			cp.y *= -1;
			auto d = cp[direction] - cursor_prev_pos[direction];
			if (direction == Vertical)
				d *= -1;
			value = prev_value + d / (size[direction] - 2 * size[1 - direction]) * max;
			if (value > max - view) value = max - view;
			else if (value < 0) value = 0;
			SendCallback(MayaEventGui::Interact);
		}
	}
}

void MayaScrollBarGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	if (background_visible) {
		g2d.UseColor(colors.Bg2);
		g2d.DrawRect(epos, size);
	}

	MayaFvec2 scp, scs;
	SlidePosAndSize(scp, scs);
	scp = scp + epos;
	scs[1 - direction] -= 8;
	g2d.UseColor(colors.Fg2);
	g2d.DrawRect(scp, scs);

	g2d.UseColor(colors.Fg2);
	g2d.UseRotation(direction ? 0 : -3.1415927f / 2);

	MayaFvec2 b0p, b0s;
	ButtonPosAndSize(0, b0p, b0s);
	b0p = b0p + epos;
	b0s[direction] *= 0.6f;
	g2d.DrawIsoTriangle(b0p, b0s * 0.6f);

	MayaFvec2 b1p, b1s;
	ButtonPosAndSize(1, b1p, b1s);
	b1p = b1p + epos;
	b1s[direction] *= 0.6f;
	b1s.y *= -1;
	g2d.DrawIsoTriangle(b1p, b1s * 0.6f);

	g2d.UseRotation(0);
}

void MayaScrollBarGui::SetScrollDirection(Direction dir)
{
	direction = dir;
}

MayaScrollBarGui::Direction MayaScrollBarGui::GetScrollDirection() const
{
	return direction;
}

void MayaScrollBarGui::SetScrollMax(float max)
{
	MAYA_DIF(max < 0)
	{
		MayaSendError({ MAYA_BOUNDARY_ERROR, "MayaScrollBarGui::SetScrollMax(float): max cannot < 0." });
		return;
	}
	this->max = max;
}

float MayaScrollBarGui::GetScrollMax() const
{
	return max;
}

void MayaScrollBarGui::SetScrollView(float view)
{
	this->view = view;
}

float MayaScrollBarGui::GetScrollView() const
{
	return view;
}

void MayaScrollBarGui::SetValue(float value)
{
	MAYA_DIF(value < 0 || value > max - view)
	{
		MayaSendError({ MAYA_BOUNDARY_ERROR,
			"MayaScrollBarGui::SetValue(float): Required value is out of bound [0, max - view]." });
		return;
	}
	this->value = value;
}

float MayaScrollBarGui::GetValue() const
{
	return value;
}

void MayaScrollBarGui::ButtonPosAndSize(int index, MayaFvec2& pos, MayaFvec2& sz)
{
	pos = { 0, 0 };
	sz = { size[1 - direction], size[1 - direction] };
	pos[direction] += (index ? -1 : 1) * (direction ? -1 : 1) * (sz[direction] - size[direction]) / 2;
}

void MayaScrollBarGui::SlidePosAndSize(MayaFvec2& pos, MayaFvec2& sz)
{
	pos = { 0, 0 };
	sz = size;
	auto bsz = size[1 - direction];

	sz[direction] -= 2 * bsz;
	sz[direction] *= view / max;
	pos[direction] += (direction ? -1 : 1)
		* ((sz[direction] - size[direction]) / 2 + bsz
			+ value / max * (size[direction] - 2 * bsz));
}