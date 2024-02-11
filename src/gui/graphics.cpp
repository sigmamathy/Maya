#include <maya/gui/graphics.hpp>
#include <maya/gui/button.hpp>
#include <maya/gui/textfield.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <maya/font.hpp>

MayaGraphicsGUI::MayaGraphicsGUI(MayaWindow& window)
	: Window(&window), g2d(window)
{
	auto projupdate = [this](MayaEvent& e) -> void
	{
		if (e.GetEventID() == MayaWindowResizedEvent::EventID)
			g2d.UseWindowProjection();
		for (auto& comp : components)
			if (comp) comp->ReactEvent(e);
	};
	
	callbackid = Window->AddEventCallback(projupdate);
	g2d.UseWindowProjection();
}

MayaGraphicsGUI::~MayaGraphicsGUI()
{
	if (MayaWindow::Exists(Window))
		Window->RemoveEventCallback(callbackid);
}

MayaGraphicsGUI::Button& MayaGraphicsGUI::CreateButton()
{
	auto button = new Button(*this);
	components.emplace_back(static_cast<Component*>(button));
	return *button;
}

MayaGraphicsGUI::TextField& MayaGraphicsGUI::CreateTextField()
{
	auto tf = new TextField(*this);
	components.emplace_back(static_cast<Component*>(tf));
	return *tf;
}

void MayaGraphicsGUI::Draw()
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i])
			components[i]->Draw(g2d);
	}
}

#include "./opensans.hpp"
MayaFont& MayaGraphicsGUI::GetDefaultFont()
{
	if (!default_font)
		default_font = MayaCreateFontUptr(*Window, s_OpenSansRegular, sizeof(s_OpenSansRegular), 30);
	return *default_font;
}

MayaGraphicsGUI::Component::Component(MayaGraphicsGUI& gui)
	: position(0), size(50), gui(&gui), color0(MayaWhite), color1(MayaBlack),
	visible(true), enabled(true),
	relativeto(0), relwpos(MayaCornerCC)
{
}

void MayaGraphicsGUI::Component::SetPositionRelativeTo(Component* comp)
{
	relativeto = comp;
	relwpos = MayaCornerCC;
}

void MayaGraphicsGUI::Component::SetPositionRelativeTo(MayaCorner relpos)
{
	relwpos = relpos;
	relativeto = 0;
}

MayaFvec2 MayaGraphicsGUI::Component::GetRelativePosition() const
{
	if (relativeto)
		return relativeto->GetPosition();
	auto hsz = gui->Window->GetSize() * 0.5f;
	MayaFvec2 d;
	d.x = ((relwpos & 0b11) - 2) * hsz.x;
	d.y = ((relwpos >> 2) - 2) * hsz.y;
	return d;
}

void MayaGraphicsGUI::Component::SetPosition(float x, float y)
{
	SetPosition(MayaFvec2{ x, y });
}

void MayaGraphicsGUI::Component::SetSize(float width, float height)
{
	SetSize(MayaFvec2{ width, height });
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

void MayaGraphicsGUI::Component::SetVisible(bool visible)
{
	this->visible = visible;
}

void MayaGraphicsGUI::Component::SetEnabled(bool enable)
{
	enabled = enable;
}

bool MayaGraphicsGUI::Component::IsVisible() const
{
	return visible;
}

bool MayaGraphicsGUI::Component::IsEnabled() const
{
	return enabled;
}

void MayaGraphicsGUI::Component::SetEventCallback(UserEventCallbackCR callback)
{
	this->callback = callback;
}