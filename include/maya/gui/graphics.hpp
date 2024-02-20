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
	/*
		0: background
		1: background (dim)
		2: border
		3: foreground
		4: foreground (dim)
	*/
	MayaIvec4 Color[16];

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

	void SetParent(class MayaContainerGui* container);
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

	void SetEventCallback(MayaFunctionCR<void(MayaEventGui&)> callback);

	bool PointInArea(MayaFvec2 pt) const;
	bool CursorInArea() const;

protected:

	MayaGraphicsGui* gui;
	MayaFvec2 position, size;
	MayaColorSchemeGui colors;
	bool visible, enabled;
	MayaFunction<void(MayaEventGui&)> callback;

	MayaContainerGui* parent;

	void SendCallback(MayaEventGui::EventType type);
};

class MayaContainerGui : public MayaComponentGui
{
public:

	MayaContainerGui(MayaGraphicsGui& gui);

	virtual void Add(MayaComponentGui& comp);
	virtual void Remove(MayaComponentGui& comp);

protected:

	MayaArrayList<MayaComponentGui*> childs;
};