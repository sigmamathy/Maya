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
#include "./render.hpp"

namespace maya
{

// Window parameters.
struct WindowParams
{
	// Desired size, -1 for default.
	Ivec2 Size = { -1, -1 };

	// Desired title.
	char const* Title = "Maya Application";

	// Desired monitor, -1 for no monitor.
	int Monitor = -1;

	// Desired samples for anti aliasing
	int MSAA = 1;

	// -------------------------- Window hints -------------------------- //

		// Can be resizable by user.
	bool Resizable = true;

	// Keep the window decoration.
	bool Decorated = true;

	// Automatically minimized when loses focus (Fullscreen only).
	bool AutoIconify = true;

	// Keeping the window always on top of other window.
	bool AlwaysOnTop = false;

	// Maximize the window.
	bool Maximized = false;

};

// Responsible for handle window process.
class Window
{
public:

	using uptr = stl::uptr<Window>;
	using sptr = stl::sptr<Window>;

	// Create window with parameters.
	Window(WindowParams const& param = WindowParams{});

	// Destroy window and underlying graphic resources.
	~Window();

	// No copy construct.
	Window(Window const&) = delete;
	Window& operator=(Window const&) = delete;

	// Create and return a uptr.
	static uptr MakeUnique(WindowParams const& param = WindowParams{});

	// Create and return a sptr.
	static sptr MakeShared(WindowParams const& param = WindowParams{});

	// Returns a render context.
	RenderContext& GetRenderContext();

	// Poll all incoming events to the event callback.
	static void PollEvents();

	// Add a custom event listener and return an ID.
	unsigned AddEventCallback(EventCallback const& callback);

	// Remove event listener based on ID.
	void RemoveEventCallback(unsigned index);

	// Set an close flag, does not actually close it.
	void RequestForClose(bool close = true);

	// Check if a close flag is enabled.
	bool IsRequestedForClose() const;

	// Swap the front buffer and back buffer.
	void SwapBuffers();

	// Set window position.
	void SetPosition(int x, int y);

	// Set window position.
	void SetPosition(Ivec2 pos);

	// Set window size.
	void SetSize(int width, int height);

	// Set window size.
	void SetSize(Ivec2 size);

	// Set window title.
	void SetTitle(char const* title);

	// Get window position.
	Ivec2 GetPosition() const;

	// Get window size.
	Ivec2 GetSize() const;

	// Get window title.
	stl::string const& GetTitle() const;

	// Ensure the aspect ratio of the window is always constant.
	void SetResizeAspectRatioLock(int x, int y);

	// Check if a key is pressed right now.
	bool IsKeyPressed(KeyCode keycode) const;

	// Check if a mouse button is pressed right now.
	bool IsMouseButtonPressed(MouseButton button) const;

	// Get the current cursor position.
	Fvec2 GetCursorPosition() const;

	// Check if the window is focused.
	bool IsFocused() const;

	// Set the window to fullscreen on a specific monitor.
	// By default resize the window to fit the monitor resolution as well.
	void SetFullscreenMonitor(int monitor, Ivec2 size = { -1, -1 });

	// Return the current fullscreen monitor, or -1 is none.
	int GetFullscreenMonitor() const;

private:

	void* resptr;
	EventCallback callback;
	stl::list<EventCallback> user_callbacks;
	stl::string title;
	int monitor;
	RenderContext rc;

	Window(nullptr_t) noexcept {};
	bool Initialize(WindowParams const& param);
	friend class RenderContext;
};

}