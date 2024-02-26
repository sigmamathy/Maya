#pragma once

#include "../2d/graphics.hpp"

class MayaGraphicsGui
{
public:

	MayaGraphicsGui(MayaWindow& window);
	MayaGraphicsGui(MayaGraphicsGui const&) = delete;
	MayaGraphicsGui& operator=(MayaGraphicsGui const&) = delete;
	~MayaGraphicsGui();

	MayaWindow* const Window;

	class MayaLabelGui& CreateLabel();
	class MayaButtonGui& CreateButton();
	class MayaTextFieldGui& CreateTextField();
	class MayaCheckboxGui& CreateCheckbox();
	class MayaPanelGui& CreatePanel();
	class MayaTitlePanelGui& CreateTitlePanel();
	class MayaScrollBarGui& CreateScrollBar();
	class MayaNumberScrollGui& CreateNumberScroll();

	void Draw();

	MayaFont& GetDefaultFont();

private:

	MayaArrayList<MayaUptr<class MayaComponentGui>> components;
	MayaGraphics2d g2d;
	unsigned callbackid;

	MayaFontUptr default_font;
};

struct MayaEventGui
{
	enum EventType {
		Interact,
		Typing,
		Entered
	} Type;
	MayaComponentGui* Source;
	MayaGraphicsGui* Gui;
};

struct MayaColorSchemeGui
{
	MayaIvec4 Bg1, Bg2, Bg3; // light, normal, dim
	MayaIvec4 Fg1, Fg2, Fg3; // light, normal, dim
	MayaIvec4 Border;

	static MayaColorSchemeGui DefaultScheme();

	MayaIvec4& operator[](int index);
	MayaIvec4 const& operator[](int index) const;
};

class MayaComponentGui
{
public:
	MayaComponentGui(MayaGraphicsGui& gui);
	virtual ~MayaComponentGui() = default;

	virtual void ReactEvent(MayaEvent& e) = 0;
	virtual void Draw(MayaGraphics2d& g2d) = 0;

	void SetParent(class MayaContainerGui* container, bool bounded = true);
	MayaContainerGui* GetParent();
	MayaFvec2 GetExactPosition() const;

	void SetPosition(float x, float y);
	void SetSize(float width, float height);
	virtual void SetPosition(MayaFvec2 pos);
	virtual void SetSize(MayaFvec2 size);
	virtual MayaFvec2 GetPosition() const;
	virtual MayaFvec2 GetSize() const;

	void SetColorScheme(MayaColorSchemeGui scheme);
	void SetColor(int index, MayaIvec4 color);
	MayaColorSchemeGui const& GetColorScheme() const;
	MayaIvec4 GetColor(int index) const;

	void SetVisible(bool visible);
	void SetEnabled(bool enable);
	bool IsVisible() const;
	bool IsEnabled() const;

	bool IsAccessible() const;

	void SetBackgroundVisible(bool visible);
	bool IsBackgroundVisible() const;

	void SetEventCallback(MayaFunctionCR<void(MayaEventGui&)> callback);

	bool PointInArea(MayaFvec2 pt, MayaFvec2 pos, MayaFvec2 size) const;
	bool PointInArea(MayaFvec2 pt) const;
	bool CursorInArea(MayaFvec2 pos, MayaFvec2 size) const;
	bool CursorInArea() const;

protected:

	MayaGraphicsGui* gui;
	MayaFvec2 position, size;
	MayaColorSchemeGui colors;
	bool visible, enabled, background_visible;
	MayaFunction<void(MayaEventGui&)> callback;

	MayaContainerGui* parent;
	bool bound_to_parent;

	void SendCallback(MayaEventGui::EventType type);
};

class MayaContainerGui : public MayaComponentGui
{
public:

	MayaContainerGui(MayaGraphicsGui& gui);

	virtual void Add(MayaComponentGui& comp);
	virtual void Remove(MayaComponentGui& comp);

	virtual void GetContentView(MayaFvec2& pos, MayaFvec2& size) const;
	virtual MayaFvec2 GetContentShift() const;

protected:

	MayaArrayList<MayaComponentGui*> childs;
};