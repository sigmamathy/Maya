#pragma once

#include "./core.hpp"
#include "./math.hpp"
#include "./event.hpp"

static struct MayaWindowParameters
{
	// Specify the size of window. -1 stands for default, which:
	// monitor < 0:			1280x720
	// fullscreen:			monitor resolution
	MayaIvec2 Size = { -1, -1 };

	// Specify the title of window, could be empty.
	MayaString Title = "Maya Application";

	// Specify a monitor for fullscreen.
	// The value stands for the priority amongst monitors connected.
	// -1: no fullscreen is required.
	int Monitor = -1;

	// Window creation hints
	bool Resizable			= true;
	bool Decorated			= true;
	bool AutoIconify		= true;
	bool AlwaysOnTop		= false;
	bool Maximized			= false;

} MayaDefaultWindowParameters;

MAYA_TYPEDEFPTR(MayaWindow);

MayaWindowUptr MayaCreateWindowUptr(MayaWindowParameters& param = MayaDefaultWindowParameters);

MayaWindowSptr MayaCreateWindowSptr(MayaWindowParameters& param = MayaDefaultWindowParameters);

void MayaPollWindowEvents();

class MayaWindow
{

public:

	MayaWindow(void* resptr, int monitor, MayaStringCR title);

	~MayaWindow();

	MayaWindow(MayaWindow const&) = delete;

	MayaWindow& operator=(MayaWindow const&) = delete;

	void SetEventCallback(MayaEventCallbackCR callback);

	void PleaseClose(bool close = true);

	bool IsTimeToClose() const;

	void UseGraphicsContext();

	void ClearBuffers();

	void ResizeViewport(MayaIvec2 pos, MayaIvec2 size);

	void PackViewport();

	void SwapBuffers();

	void SetPosition(MayaIvec2 pos);

	void SetSize(MayaIvec2 size);

	void SetTitle(MayaStringCR title);

	MayaIvec2 GetPosition() const;

	MayaIvec2 GetSize() const;

	MayaString GetTitle() const;

	void SetResizeAspectRatioLock(int x, int y);

	bool IsKeyPressed(MayaKeyCode keycode) const;

	bool IsMouseButtonPressed(MayaMouseButton button) const;

	MayaFvec2 GetCursorPosition() const;

	bool IsFocused() const;

	void SetFullscreenMonitor(int monitor, MayaIvec2 size = {-1, -1});

	int GetFullscreenMonitor() const;

	class MayaGraphics2D& GetGraphics2D();

	class MayaGraphicsGUI& GetGraphicsGUI();

private:

	void* resptr;
	MayaEventCallback event_callback;
	MayaString title;
	int monitor;

	MayaUptr<MayaGraphics2D> graphics2d;
	MayaUptr<MayaGraphicsGUI> graphicsgui;

private:

	void UpdateGraphicsGUIEventIfPresent(MayaEvent& e);
};