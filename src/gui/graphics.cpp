#include <maya/gui/graphics.hpp>
#include <maya/gui/button.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <maya/font.hpp>

MayaGraphicsGUI::MayaGraphicsGUI(MayaWindow& window)
	: Window(&window), g2d(window)
{
	auto projupdate = [this](MayaEvent& e) -> void
	{
		if (e.GetEventID() == MayaWindowResizedEvent::EventID) {
			auto re = static_cast<MayaWindowResizedEvent*>(&e);
			g2d.UseProjection(re->Size);
		}
		for (auto& comp : components)
			if (comp) comp->ReactEvent(e);
	};
	
	Window->AddEventCallback(projupdate);
	auto sz = Window->GetSize();
	g2d.UseProjection(sz);
}

MayaGraphicsGUI::Button& MayaGraphicsGUI::CreateButton()
{
	auto button = new Button(*this);
	components.emplace_back(static_cast<Component*>(button));
	return *button;
}

void MayaGraphicsGUI::Draw()
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i])
			components[i]->Draw(g2d);
	}
}

MayaGraphicsGUI::Component::Component(MayaGraphicsGUI& gui)
	: position(0), size(50), gui(&gui), color0(MayaWhite), color1(MayaBlack), relativeto(0), relwpos(RelativeToNone)
{
}

void MayaGraphicsGUI::Component::SetPositionRelativeTo(Component* comp)
{
	relativeto = comp;
	relwpos = RelativeToNone;
}

void MayaGraphicsGUI::Component::SetPositionRelativeTo(WindowRelativePos relpos)
{
	relwpos = relpos;
	relativeto = 0;
}

MayaFvec2 MayaGraphicsGUI::Component::GetRelativePosition() const
{
	if (relativeto)
		return relativeto->GetPosition();
	auto hsz = gui->Window->GetSize() * 0.5f;
	switch (relwpos)
	{
		case RelativeToWindowTopLeft:
			return MayaFvec2(-hsz.x, hsz.y);
		case RelativeToWindowTopCenter:
			return MayaFvec2(0, hsz.y);
		case RelativeToWindowTopRight:
			return MayaFvec2(hsz.x, hsz.y);
		case RelativeToWindowCenterLeft:
			return MayaFvec2(-hsz.x, 0);
		case RelativeToNone:
			return MayaFvec2(0, 0);
		case RelativeToWindowCenterRight:
			return MayaFvec2(hsz.x, 0);
		case RelativeToWindowBottomLeft:
			return MayaFvec2(-hsz.x, -hsz.y);
		case RelativeToWindowBottomCenter:
			return MayaFvec2(0, -hsz.y);
		case RelativeToWindowBottomRight:
			return MayaFvec2(hsz.x, -hsz.y);
	}
	return {};
}

void MayaGraphicsGUI::Component::SetPosition(MayaFvec2 pos)
{
	position = pos;
}

void MayaGraphicsGUI::Component::SetSize(MayaFvec2 size)
{
	this->size = size;
}

MayaFvec2 MayaGraphicsGUI::Component::GetPosition() const
{
	return position;
}

MayaFvec2 MayaGraphicsGUI::Component::GetSize() const
{
	return size;
}

void MayaGraphicsGUI::Component::SetColor0(MayaIvec4 color)
{
	color0 = color;
}

void MayaGraphicsGUI::Component::SetColor1(MayaIvec4 color)
{
	color1 = color;
}

MayaIvec4 MayaGraphicsGUI::Component::GetColor0() const
{
	return color0;
}

MayaIvec4 MayaGraphicsGUI::Component::GetColor1() const
{
	return color1;
}