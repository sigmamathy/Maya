#include <maya/gui/scrollbar.hpp>

MayaScrollBarGui::MayaScrollBarGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), direction(Vertical), max(1), value(0),
	cursor_clicked(0), cursor_prev_pos(0), prev_value(0)
{
	size = { 10, 300 };
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
				MayaFvec2 scp(0);
				auto scs = size;
				scs[direction] /= max + 1;
				scp[direction] = (direction ? -1 : 1) * ((scs[direction] - size[direction]) / 2 + value * scs[direction]);

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
			value = prev_value + d / size[direction] * (max + 1);
			if (value > max) value = max;
			else if (value < 0) value = 0;
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

	auto scp = epos;
	auto scs = size;
	scs[direction] /= max + 1;
	scp[direction] += (direction ? -1 : 1) * ((scs[direction] - size[direction]) / 2 + value * scs[direction]);

	g2d.UseColor(colors.Bg1);
	g2d.DrawRect(scp, scs);
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

void MayaScrollBarGui::SetValue(float value)
{
	MAYA_DIF(value < 0 || value > max)
	{
		MayaSendError({ MAYA_BOUNDARY_ERROR, "MayaScrollBarGui::SetValue(float): Required value is out of bound [0, max]." });
		return;
	}
	this->value = value;
}

float MayaScrollBarGui::GetValue() const
{
	return value;
}