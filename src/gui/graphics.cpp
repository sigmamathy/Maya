#include <maya/gui/graphics.hpp>
#include <maya/gui/label.hpp>
#include <maya/gui/button.hpp>
#include <maya/gui/textfield.hpp>
#include <maya/gui/checkbox.hpp>
#include <maya/color.hpp>
#include <maya/transformation.hpp>
#include <maya/font.hpp>

MayaGraphicsGui::MayaGraphicsGui(MayaWindow& window)
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

MayaGraphicsGui::~MayaGraphicsGui()
{
	if (MayaWindow::Exists(Window))
		Window->RemoveEventCallback(callbackid);
}

#define MAYA_DEFINE_CREATE(x, y) x& MayaGraphicsGui::Create##y()\
	{ auto comp = new x(*this); components.emplace_back(static_cast<MayaComponentGui*>(comp)); return *comp; }

MAYA_DEFINE_CREATE(MayaLabelGui, Label)
MAYA_DEFINE_CREATE(MayaButtonGui, Button)
MAYA_DEFINE_CREATE(MayaTextFieldGui, TextField)
MAYA_DEFINE_CREATE(MayaCheckboxGui, Checkbox)

void MayaGraphicsGui::Draw()
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i])
			components[i]->Draw(g2d);
	}
}

#include "./resources/opensans"

MayaFont& MayaGraphicsGui::GetDefaultFont()
{
	if (!default_font)
		default_font = MayaCreateFontUptr(*Window, s_open_sans_regular, sizeof(s_open_sans_regular), 30);
	return *default_font;
}

MayaColorSchemeGui MayaColorSchemeGui::DefaultScheme()
{
	static MayaColorSchemeGui scheme = {
		.Color = {
			{ 90, 101, 107, 255 },
			{ 65, 71, 74, 255 },
			{ 109, 123, 130, 255 },
			MayaWhite,
			MayaGray,
		}
	};

	return scheme;
}

MayaIvec4& MayaColorSchemeGui::operator[](int index)
{
	return Color[index];
}

MayaIvec4 const& MayaColorSchemeGui::operator[](int index) const
{
	return Color[index];
}

MayaComponentGui::MayaComponentGui(MayaGraphicsGui& gui)
	: position(0), size(50), gui(&gui),
	visible(true), enabled(true),
	relativeto(0), relwpos(MayaCornerCC)
{
	colors = MayaColorSchemeGui::DefaultScheme();
}

void MayaComponentGui::SetPositionRelativeTo(MayaComponentGui* comp)
{
	relativeto = comp;
	relwpos = MayaCornerCC;
}

void MayaComponentGui::SetPositionRelativeTo(MayaCorner relpos)
{
	relwpos = relpos;
	relativeto = 0;
}

MayaFvec2 MayaComponentGui::GetRelativePosition() const
{
	if (relativeto)
		return relativeto->GetPosition() + relativeto->GetRelativePosition();
	auto hsz = gui->Window->GetSize() * 0.5f;
	MayaFvec2 d;
	d.x = ((relwpos & 0b11) - 2) * hsz.x;
	d.y = ((relwpos >> 2) - 2) * hsz.y;
	return d;
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