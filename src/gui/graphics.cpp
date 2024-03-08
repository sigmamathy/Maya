#include <maya/gui/graphics.hpp>
#include <maya/gui/label.hpp>
#include <maya/gui/button.hpp>
#include <maya/gui/textfield.hpp>
#include <maya/gui/checkbox.hpp>
#include <maya/gui/panel.hpp>
#include <maya/gui/scrollbar.hpp>
#include <maya/gui/numberscroll.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <maya/font.hpp>

MayaGraphicsGui::MayaGraphicsGui(Window& window)
	: Window(&window), g2d(window)
{
	components.reserve(30);

	auto projupdate = [this](MayaEvent& e) -> void
	{
		if (e.GetEventID() == MayaWindowResizedEvent::EventID)
			g2d.UseWindowProjection();
		for (auto& comp : components) {
			if (comp && comp->IsAccessible())
				comp->ReactEvent(e);
		}
	};
	
	callbackid = Window->AddEventCallback(projupdate);
	g2d.UseWindowProjection();
}

MayaGraphicsGui::~MayaGraphicsGui()
{
	if (Window::Exists(Window))
		Window->RemoveEventCallback(callbackid);
}

#define MAYA_DEFINE_CREATE(x) Maya##x##Gui& MayaGraphicsGui::Create##x()\
	{ auto comp = new Maya##x##Gui(*this); components.emplace_back(static_cast<MayaComponentGui*>(comp)); return *comp; }

MAYA_DEFINE_CREATE(Label)
MAYA_DEFINE_CREATE(Button)
MAYA_DEFINE_CREATE(TextField)
MAYA_DEFINE_CREATE(Checkbox)
MAYA_DEFINE_CREATE(Panel)
MAYA_DEFINE_CREATE(TitlePanel)
MAYA_DEFINE_CREATE(ScrollBar)
MAYA_DEFINE_CREATE(NumberScroll)

void MayaGraphicsGui::Draw()
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i] && !components[i]->GetParent())
			components[i]->Draw(g2d);
	}
}

#include "./resources/opensans"

Font& MayaGraphicsGui::GetDefaultFont()
{
	if (!default_font)
		default_font = MayaCreateFontUptr(*Window, s_open_sans_regular, sizeof(s_open_sans_regular), 30);
	return *default_font;
}

MayaColorSchemeGui MayaColorSchemeGui::DefaultScheme()
{
	static MayaColorSchemeGui scheme = {
		.Bg1 = { 90, 101, 107, 255 },
		.Bg2 = { 65, 71, 74, 255 },
		.Bg3 = { 49, 51, 54, 255 },
		.Fg1 = MayaWhite,
		.Fg2 = MayaGray,
		.Fg3 = { 94, 94, 94, 255 },
		.Border = { 109, 123, 130, 255 }
	};

	return scheme;
}

MayaIvec4& MayaColorSchemeGui::operator[](int index)
{
	switch (index) {
		case 0: return Bg1;
		case 1: return Bg2;
		case 2: return Bg3;
		case 3: return Fg1;
		case 4: return Fg2;
		case 5: return Fg3;
		case 6: return Border;
	}

	MayaSendError({ MAYA_BOUNDARY_ERROR,
		"MayaColorSchemeGui::operator[](int): Not a valid index for color scheme." });
	return Bg1;
}

MayaIvec4 const& MayaColorSchemeGui::operator[](int index) const
{
	switch (index) {
		case 0: return Bg1;
		case 1: return Bg2;
		case 2: return Bg3;
		case 3: return Fg1;
		case 4: return Fg2;
		case 5: return Fg3;
		case 6: return Border;
	}

	MayaSendError({ MAYA_BOUNDARY_ERROR,
		"MayaColorSchemeGui::operator[](int) const: Not a valid index for color scheme." });
	return Bg1;
}

MayaComponentGui::MayaComponentGui(MayaGraphicsGui& gui)
	: position(0), size(50), gui(&gui),
	visible(true), enabled(true),
	parent(0), background_visible(1), bound_to_parent(1)
{
	colors = MayaColorSchemeGui::DefaultScheme();
}

void MayaComponentGui::SetParent(MayaContainerGui* container, bool bounded)
{
	parent = container;
	bound_to_parent = bounded;
}

MayaContainerGui* MayaComponentGui::GetParent()
{
	return parent;
}

MayaFvec2 MayaComponentGui::GetExactPosition() const
{
	if (parent)
	{
		MayaFvec2 res = parent->GetExactPosition() + position;
		return bound_to_parent ? res + parent->GetContentShift() : res;
	}
	return position;
}

void MayaComponentGui::SetPosition(float x, float y)
{
	SetPosition(MayaFvec2{ x, y });
}

void MayaComponentGui::SetSize(float width, float height)
{
	SetSize(MayaFvec2{ width, height });
}

void MayaComponentGui::SetPosition(MayaFvec2 pos)
{
	position = pos;
}

void MayaComponentGui::SetSize(MayaFvec2 size)
{
	this->size = size;
}

MayaFvec2 MayaComponentGui::GetPosition() const
{
	return position;
}

MayaFvec2 MayaComponentGui::GetSize() const
{
	return size;
}

void MayaComponentGui::SetColorScheme(MayaColorSchemeGui scheme)
{
	colors = scheme;
}

void MayaComponentGui::SetColor(int index, MayaIvec4 color)
{
	colors[index] = color;
}

MayaColorSchemeGui const& MayaComponentGui::GetColorScheme() const
{
	return colors;
}

MayaIvec4 MayaComponentGui::GetColor(int index) const
{
	return colors[index];
}

void MayaComponentGui::SetVisible(bool visible)
{
	this->visible = visible;
}

void MayaComponentGui::SetEnabled(bool enable)
{
	enabled = enable;
}

bool MayaComponentGui::IsVisible() const
{
	return visible;
}

bool MayaComponentGui::IsEnabled() const
{
	return enabled;
}

bool MayaComponentGui::IsAccessible() const
{
	if (parent)
		return parent->IsAccessible() && visible && enabled;
	return visible && enabled;
}

void MayaComponentGui::SetBackgroundVisible(bool visible)
{
	background_visible = visible;
}

bool MayaComponentGui::IsBackgroundVisible() const
{
	return background_visible;
}

void MayaComponentGui::SetEventCallback(MayaFunctionCR<void(MayaEventGui&)> callback)
{
	this->callback = callback;
}

void MayaComponentGui::SendCallback(MayaEventGui::EventType type)
{
	if (!callback)
		return;
	MayaEventGui e;
	e.Type = type;
	e.Source = this;
	e.Gui = gui;
	callback(e);
}

bool MayaComponentGui::PointInArea(MayaFvec2 pt, MayaFvec2 pos, MayaFvec2 size) const
{
	if (parent) {
		pt = pt - parent->GetExactPosition();
		if (bound_to_parent) {
			MayaFvec2 pp, ps;
			parent->GetContentView(pp, ps);
			pt = pt - pp;
			bool inparent = pt.x >= -ps.x * 0.5f && pt.x <= ps.x * 0.5f
				&& pt.y >= -ps.y * 0.5f && pt.y <= ps.y * 0.5f;
			if (!inparent)
				return false;
			pt = pt + pp;
			pt = pt - parent->GetContentShift();
		}
	}

	pt = pt - pos - position;
	return pt.x >= -size.x * 0.5f && pt.x <= size.x * 0.5f
		&& pt.y >= -size.y * 0.5f && pt.y <= size.y * 0.5f;
}

bool MayaComponentGui::PointInArea(MayaFvec2 pt) const
{
	return PointInArea(pt, MayaFvec2(0), size);
}

bool MayaComponentGui::CursorInArea(MayaFvec2 pos, MayaFvec2 size) const
{
	auto* window = gui->Window;
	MayaFvec2 cp = window->GetCursorPosition();
	cp = cp - window->GetSize() / 2;
	cp.y = -cp.y;
	return PointInArea(cp, pos, size);
}

bool MayaComponentGui::CursorInArea() const
{
	auto* window = gui->Window;
	MayaFvec2 cp = window->GetCursorPosition();
	cp = cp - window->GetSize() / 2;
	cp.y = -cp.y;
	return PointInArea(cp);
}

MayaContainerGui::MayaContainerGui(MayaGraphicsGui& gui)
	: MayaComponentGui(gui)
{
}

void MayaContainerGui::Add(MayaComponentGui& comp)
{
	childs.emplace_back(&comp);
	comp.SetParent(this);
}

void MayaContainerGui::Remove(MayaComponentGui& comp)
{
	comp.SetParent(nullptr);
	childs.erase(std::remove(childs.begin(), childs.end(), &comp), childs.end());
}

void MayaContainerGui::GetContentView(MayaFvec2& pos, MayaFvec2& size) const
{
	pos = {0, 0};
	size = this->size;
}

MayaFvec2 MayaContainerGui::GetContentShift() const
{
	return { 0, 0 };
}