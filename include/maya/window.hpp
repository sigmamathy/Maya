/**
	@file
	@author		Sigmamathy
	@version	1.0

	@section LICENSE

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	General Public License for more details at
	https://www.gnu.org/copyleft/gpl.html

 */

#pragma once

#include "./core.hpp"
#include "./math.hpp"
#include "./event.hpp"

/**
	@brief Initial configuration for window creation. (Optional)

	This struct provides user customization on window that
	will be immediately applied at the moment the window is created.
	Some option can be changed later (e.g. size and title) but some not,
	which may require destroy the existing window and create a new one.
	All parameters is default initialized.

	@see MayaCreateWindowUptr(MayaWindowParameters&)
	@see MayaCreateWindowSptr(MayaWindowParameters&)
*/
static struct MayaWindowParameters
{
	/**
		@brief Desired size of window.

		Setting this field will change the size of window,
		which is relative to the screen resolution.
		The window can also be resized again by MayaWindow::SetSize.
		Value of negative one stands for default option, which is 1280x720
		if the window is in windowed mode, or the resolution of monitor selected
		if fullscreen window is desired.

		@see MayaWindow::SetSize(MayaIvec2)
	*/
	MayaIvec2 Size = { -1, -1 };

	/**
		@brief Desired title of the window.

		This title parameter refers to the text visible on the window decoration.
		This string could be empty. Can also be modified by MayaWindow::SetTitle.
		No use if window is in fullscreen or undecorated.

		@see MayaWindow::SetTitle
	*/
	MayaString Title = "Maya Application";

	/**
		@brief Desired monitor for fullscreen window.
		
		This parameter indicates which monitor should the window used for
		fullscreen window, by using the display number assigned by operating system.
		For instance, zero is the primary monitor and one is the secondary monitor.
		Value of negative one indicates windowed mode is desired.
		Can be reset by MayaWindow::SetFullscreenMonitor.

		@see MayaWindow::SetFullscreenMonitor
	*/
	int Monitor = -1;

// -------------------------- Window hints -------------------------- //

	/**
		@brief Set whether the window should be resizable by user.

		If set to false, the user will no longer be able to resize the window,
		including maximizing the window.
		The window can still be resized in code without limits.
		Cannot be changed once the window is constructed.
	*/
	bool Resizable			= true;

	/**
		@brief Options for keeping the window decorated.

		If set to false, the top decoration will disappear to user,
		which also means that the window cannot be freely moved by user anymore.
		Cannot be changed once the window is constructed.
	*/
	bool Decorated			= true;

	/**
		@brief Automatically minimized when loses focus (Fullscreen only).

		Refers to the behaviour where a window is automatically minimized (iconified)
		by the window system when it loses focus.
		No use for window in windowed mode
		Cannot be changed once the window is constructed.
	*/
	bool AutoIconify		= true;

	/**
		@brief Keeping the window always on top of other window.

		The window will always on top of other window even if it loses foucs.
		Also known as FLOATING in glfw contexts.
		Cannot be changed once the window is constructed.
	*/
	bool AlwaysOnTop		= false;

	/**
		@brief Maximize the window.

		Set the window to maximized mode when created.
		Unknown behaviour if SetSize is called when the window is maximized,
		or applied when window is in fullscreen.
		This option can always be changed later, both by user or code.
	*/
	bool Maximized			= false;

}

/**
	Default parameters for this struct.
	Equivalent to MayaWindowParameters{}.
*/
MayaDefaultWindowParameters;

// Window Pointers typedef
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

	static bool Exists(MayaWindow* window);

	unsigned AddEventCallback(MayaEventCallbackCR callback);

	void RemoveEventCallback(unsigned index);

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

private:

	void* resptr;
	MayaEventCallback event_callback;
	MayaArrayList<MayaEventCallback> callbacks;
	MayaString title;
	int monitor;

};