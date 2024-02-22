#include <maya/window.hpp>
#include <maya/deviceinfo.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <maya/renderer.hpp>
#include <unordered_set>

static void s_SetupWindowEventCallback(GLFWwindow* window)
{
	glfwSetKeyCallback(window,
	[](GLFWwindow* window, int key, int scancode, int act, int modes) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaKeyEvent e;
		e.KeyCode = static_cast<MayaKeyCode>(key);
		e.Down = act != GLFW_RELEASE;
		e.Repeat = act == GLFW_REPEAT;
		e.Mods = static_cast<MayaModifierKeys>(modes);
		callback(e);
	});

	glfwSetMouseButtonCallback(window,
	[](GLFWwindow* window, int button, int act, int modes) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaMouseEvent e;
		e.Button = static_cast<MayaMouseButton>(button);
		e.Down = act != GLFW_RELEASE;
		e.Mods = static_cast<MayaModifierKeys>(modes);
		callback(e);
	});

	glfwSetCursorPosCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaMouseMovedEvent e;
		e.Position = { (int) x, (int) y };
		callback(e);
	});

	glfwSetScrollCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaMouseScrolledEvent e;
		e.Offset = { (int) x, (int) y };
		callback(e);
	});

	glfwSetWindowFocusCallback(window,
	[](GLFWwindow* window, int focus) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaWindowFocusedEvent e;
		e.Focused = focus;
		callback(e);
	});

	glfwSetWindowCloseCallback(window,
	[](GLFWwindow* window) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaWindowClosedEvent e;
		callback(e);
	});

	glfwSetWindowSizeCallback(window,
	[](GLFWwindow* window, int width, int height) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaWindowResizedEvent e;
		e.Size = { width, height };
		callback(e);
	});

	glfwSetWindowPosCallback(window,
	[](GLFWwindow* window, int x, int y) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaWindowMovedEvent e;
		e.Position = { x, y };
		callback(e);
	});

	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned codepoint) {
		auto& callback = *static_cast<MayaEventCallback*>(glfwGetWindowUserPointer(window));
		MayaCharEvent e;
		e.Char = (char) codepoint;
		callback(e);
	});
}

static MayaWindow* s_CreateWindowPtr(MayaWindowParameters& param)
{	
	glfwWindowHint(GLFW_RESIZABLE,		param.Resizable);
	glfwWindowHint(GLFW_DECORATED,		param.Decorated);
	glfwWindowHint(GLFW_AUTO_ICONIFY,	param.AutoIconify);
	glfwWindowHint(GLFW_FLOATING,		param.AlwaysOnTop);
	glfwWindowHint(GLFW_MAXIMIZED,		param.Maximized);

	GLFWwindow* window = 0;

	if (param.Monitor == -1)
	{
		int sx = param.Size.x, sy = param.Size.y;
		if (sx == -1) sx = 1280;
		if (sy == -1) sy = 720;
		window = glfwCreateWindow(sx, sy, param.Title.c_str(), nullptr, nullptr);
	}
	else {
		int sx = param.Size.x, sy = param.Size.y;
		MayaMonitorsInfo info;
		MayaGetDeviceInfo(&info);
		auto m = info.Monitors[param.Monitor];
		if (sx == -1) sx = m.Resolution.x;
		if (sy == -1) sy = m.Resolution.y;
		window = glfwCreateWindow(sx, sy, param.Title.c_str(),
			static_cast<GLFWmonitor*>(m.Resptr), nullptr);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	return new MayaWindow(window, param.Monitor, param.Title);
}

MayaWindowUptr MayaCreateWindowUptr(MayaWindowParameters& param)
{
	MAYA_DIF(!MayaGetLibraryManager()
		|| !MayaGetLibraryManager()->FoundDependency(MAYA_LIBRARY_GLFW))
	{
		MayaSendError({ MAYA_MISSING_LIBRARY_ERROR,
			"MayaCreateWindowUptr(MayaWindowParameters&): GLFW is not initialized." });
		return nullptr;
	}

	return MayaWindowUptr(s_CreateWindowPtr(param));
}

MayaWindowSptr MayaCreateWindowSptr(MayaWindowParameters& param)
{
	MAYA_DIF(!MayaGetLibraryManager()
		|| !MayaGetLibraryManager()->FoundDependency(MAYA_LIBRARY_GLFW))
	{
		MayaSendError({ MAYA_MISSING_LIBRARY_ERROR,
			"MayaCreateWindowSptr(MayaWindowParameters&): GLFW is not initialized." });
		return nullptr;
	}

	return MayaWindowSptr(s_CreateWindowPtr(param));
}

static std::unordered_set<MayaWindow*> s_window_instances;

MayaWindow::MayaWindow(void* pointer, int monitor, MayaStringCR title)
	: resptr(pointer), monitor(monitor), title(title)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	event_callback = [this](MayaEvent& e) {
		e.Window = this;
		for (auto& call : callbacks)
			if(call) call(e);
	};
	glfwSetWindowUserPointer(window, &event_callback);
	s_SetupWindowEventCallback(window);
	s_window_instances.insert(this);
	extern void Maya_s_InitCache(MayaWindow* window);
	Maya_s_InitCache(this);
}

MayaWindow::~MayaWindow()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwDestroyWindow(window);
	s_window_instances.erase(this);
}

bool MayaWindow::Exists(MayaWindow* window)
{
	return s_window_instances.contains(window);
}

unsigned MayaWindow::AddEventCallback(MayaEventCallbackCR callback)
{
	callbacks.push_back(callback);
	return static_cast<unsigned>(callbacks.size() - 1);
}

void MayaWindow::RemoveEventCallback(unsigned index)
{
	callbacks.at(index) = nullptr;
}

void MayaWindow::PleaseClose(bool close)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSetWindowShouldClose(window, close);
}

bool MayaWindow::IsTimeToClose() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	return glfwWindowShouldClose(window);
}

void MayaWindow::UseGraphicsContext()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwMakeContextCurrent(window);
}

void MayaWindow::ClearBuffers()
{
	UseGraphicsContext();
	extern void Maya_s_BindScissorTest(MayaWindow* window, MayaScissorTest* test);
	Maya_s_BindScissorTest(this, 0);
}

void MayaWindow::ResizeViewport(MayaIvec2 pos, MayaIvec2 size)
{
	UseGraphicsContext();
	glViewport(pos.x, GetSize().y - pos.y - size.y, size.x, size.y);
}

void MayaWindow::PackViewport()
{
	UseGraphicsContext();
	auto size = GetSize();
	glViewport(0, 0, size.x, size.y);
}

void MayaPollWindowEvents()
{
	glfwPollEvents();
}

void MayaWindow::SwapBuffers()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSwapBuffers(window);
}

void MayaWindow::SetPosition(int x, int y)
{
	SetPosition(MayaIvec2(x, y));
}

void MayaWindow::SetPosition(MayaIvec2 pos)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSetWindowPos(window, pos.x, pos.y);
}

void MayaWindow::SetSize(int width, int height)
{
	SetSize(MayaIvec2(width, height));
}

void MayaWindow::SetSize(MayaIvec2 size)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSetWindowSize(window, size.x, size.y);
}

void MayaWindow::SetTitle(MayaStringCR title)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSetWindowTitle(window, title.c_str());
	this->title = title;
}

MayaIvec2 MayaWindow::GetPosition() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	MayaIvec2 result;
	glfwGetWindowPos(window, &result.x, &result.y);
	return result;
}

MayaIvec2 MayaWindow::GetSize() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	MayaIvec2 result;
	glfwGetWindowSize(window, &result.x, &result.y);
	return result;
}

MayaString MayaWindow::GetTitle() const
{
	return title;
}

void MayaWindow::SetResizeAspectRatioLock(int x, int y)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	glfwSetWindowAspectRatio(window, x, y);
}

bool MayaWindow::IsKeyPressed(MayaKeyCode keycode) const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	return glfwGetKey(window, keycode) == GLFW_PRESS;
}

bool MayaWindow::IsMouseButtonPressed(MayaMouseButton button) const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

MayaFvec2 MayaWindow::GetCursorPosition() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return { x, y };
}

bool MayaWindow::IsFocused() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void MayaWindow::SetFullscreenMonitor(int monitor, MayaIvec2 size)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(resptr);
	this->monitor = monitor;
	if (monitor == -1) {
		glfwSetWindowMonitor(window, nullptr, 0, 0, size.x != -1 ? size.x : 1280, size.y != -1 ? size.y : 720, GLFW_DONT_CARE);
		return;
	}

	MayaMonitorsInfo info;
	MayaGetDeviceInfo(&info);
	auto& m = info.Monitors[monitor];
	glfwSetWindowMonitor(window, static_cast<GLFWmonitor*>(m.Resptr), 0, 0,
		size.x != -1 ? size.x : m.Resolution.x, size.y != -1 ? size.y : m.Resolution.y, GLFW_DONT_CARE);
}

int MayaWindow::GetFullscreenMonitor() const
{
	return monitor;
}