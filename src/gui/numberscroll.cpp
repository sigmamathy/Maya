#include <maya/gui/numberscroll.hpp>
#include <sstream>
#include <iomanip>

MayaNumberScrollGui::MayaNumberScrollGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui), text(gui.GetDefaultFont(), "0.00"), range(-5, 5), value(0), cursor_clicked(0), precision(2)
{
	size = { 150, 60 };
	text.SetTextAlign(MayaCornerCC);
}

void MayaNumberScrollGui::ReactEvent(MayaEvent& e)
{
	if (auto* me = MayaEventCast<MayaMouseEvent>(e))
	{
		if (me->Button == MayaMouseButton1)
		{
			if (me->Down)
			{
				auto cp = e.Window->GetCursorPosition() - e.Window->GetSize() / 2;
				cp.y *= -1;
				if (PointInArea(cp))
				{
					cp = cp - GetExactPosition();
					SetValue((cp.x / size.x + 0.5f) * (range.y - range.x) + range.x);
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
			cp = cp - GetExactPosition();
			float val = (cp.x / size.x + 0.5f) * (range.y - range.x) + range.x;
			if (val > range.y) val = range.y;
			else if (val < range.x) val = range.x;
			SetValue(val);
		}
	}
}

void MayaNumberScrollGui::Draw(MayaGraphics2d& g2d)
{
	auto epos = GetExactPosition();

	if (background_visible)
	{
		g2d.UseColor(colors.Bg2);
		float v = (value - range.x) / (range.y - range.x);
		g2d.DrawRect(epos + MayaFvec2(size.x, 0) * (v - 1) * 0.5f,
			MayaFvec2(size.x * v, size.y));
	}

	g2d.UseColor(colors.Fg1);
	text.SetPosition(epos);
	g2d.DrawText(text);
	g2d.UseColor(colors.Border);
	g2d.DrawRectBorder(epos, size);
}

void MayaNumberScrollGui::SetRange(float min, float max)
{
	range = { min, max };
	if (value < min) value = min;
	else if (value > max) value = max;
}

MayaFvec2 MayaNumberScrollGui::GetRange() const
{
	return range;
}

static constexpr float s_Exp10(int ex)
{
	float result = 1.0;
	if (ex >= 0) {
		for (int i = 0; i < ex; ++i) {
			result *= 10.f;
		}
	}
	else {
		for (int i = 0; i < -ex; ++i) {
			result /= 10.f;
		}
	}
	return result;
}

void MayaNumberScrollGui::SetValue(float value)
{
	float m = s_Exp10(precision);
	this->value = std::roundf(value * m) / m;
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	text = stream.str();
}

float MayaNumberScrollGui::GetValue() const
{
	return value;
}

void MayaNumberScrollGui::SetPrecision(int num)
{
	precision = num;
}

int MayaNumberScrollGui::GetPrecision() const
{
	return precision;
}