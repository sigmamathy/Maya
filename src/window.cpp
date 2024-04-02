#include <maya/window.hpp>
#include <maya/deviceinfo.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_set>

namespace maya
{

static void s_SetupWindowEventCallback(GLFWwindow* window)
{
	glfwSetKeyCallback(window,
		[](GLFWwindow* window, int key, int scancode, int act, int modes) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			KeyEvent e;
			e.Keycode = static_cast<KeyCode>(key);
			e.Down = act != GLFW_RELEASE;
			e.Repeat = act == GLFW_REPEAT;
			e.Mods = static_cast<ModifierKeys>(modes);
			callback(e);
		});

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow* window, int button, int act, int modes) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			MouseEvent e;
			e.Button = static_cast<MouseButton>(button);
			e.Down = act != GLFW_RELEASE;
			e.Mods = static_cast<ModifierKeys>(modes);
			callback(e);
		});

	glfwSetCursorPosCallback(window,
		[](GLFWwindow* window, double x, double y) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			MouseMovedEvent e;
			e.Position = { (int)x, (int)y };
			callback(e);
		});

	glfwSetScrollCallback(window,
		[](GLFWwindow* window, double x, double y) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			MouseScrolledEvent e;
			e.Offset = { (int)x, (int)y };
			callback(e);
		});

	glfwSetWindowFocusCallback(window,
		[](GLFWwindow* window, int focus) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			WindowFocusedEvent e;
			e.Focused = focus;
			callback(e);
		});

	glfwSetWindowCloseCallback(window,
		[](GLFWwindow* window) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			WindowClosedEvent e;
			callback(e);
		});

	glfwSetWindowSizeCallback(window,
		[](GLFWwindow* window, int width, int height) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			WindowResizedEvent e;
			e.Size = { width, height };
			callback(e);
		});

	glfwSetWindowPosCallback(window,
		[](GLFWwindow* window, int x, int y) {
			auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
			WindowMovedEvent e;
			e.Position = { x, y };
			callback(e);
		});

	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned codepoint) {
		auto& callback = *static_cast<EventCallback*>(glfwGetWindowUserPointer(window));
		CharEvent e;
		e.Char = (char)codepoint;
		callback(e);
		});
}

bool Window::Initialize(WindowParams const& param)
{
	MAYA_DEBUG_LOG_INFO("Initializing window...");

	glfwWindowHint(GLFW_RESIZABLE, param.Resizable);
	glfwWindowHint(GLFW_DECORATED, param.Decorated);
	glfwWindowHint(GLFW_AUTO_ICONIFY, param.AutoIconify);
	glfwWindowHint(GLFW_FLOATING, param.AlwaysOnTop);
	glfwWindowHint(GLFW_MAXIMIZED, param.Maximized);
	glfwWindowHint(GLFW_SAMPLES, param.MSAA);

	GLFWwindow* window = 0;

	if (param.Monitor == -1)
	{
		int sx = param.Size.x, sy = param.Size.y;
		if (sx == -1) sx = 1280;
		if (sy == -1) sy = 720;
		window = glfwCreateWindow(sx, sy, param.Title, nullptr, nullptr);
	}
	else {
		int sx = param.Size.x, sy = param.Size.y;
		auto info = MonitorsInfo::Retrieve();
		auto m = info.Monitors[param.Monitor];
		if (sx == -1) sx = m.Resolution.x;
		if (sy == -1) sy = m.Resolution.y;
		window = glfwCreateWindow(sx, sy, param.Title,
			static_cast<GLFWmonitor*>(m.Resptr), nullptr);
	}

	if (!window)
		return false;

	glfwMakeContextCurrent(window);
	gladLoadGL();
	if (param.MSAA > 1) glEnable(GL_MULTISAMPLE);
	glfwSetWindowUserPointer(window, &callback);
	s_SetupWindowEventCallback(window);

	nativeptr = window;
	monitor = param.Monitor;
	title = param.Title;
	callback = [this](Event& e) {
		e.Window = this;
		for (auto& call : user_callbacks)
			if (call) call(e);
	};


	rc.Init(this);
	return true;
}

Window::Window(WindowParams const& param)
{
	Initialize(param);
}

Window::~Window()
{
	rc.Free();
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwDestroyWindow(window);
}

Window::uptr Window::MakeUnique(WindowParams const& param)
{
	Window* window = new Window(0);
	bool res = window->Initialize(param);
	if (res) return Window::uptr(window);
	delete window;
	return 0;
}

Window::sptr Window::MakeShared(WindowParams const& param)
{
	Window* window = new Window(0);
	bool res = window->Initialize(param);
	if (res) return Window::sptr(window);
	delete window;
	return 0;
}

void* Window::GetNativePointer()
{
	return nativeptr;
}

RenderContext& Window::GetRenderContext()
{
	return rc;
}

unsigned Window::AddEventCallback(EventCallback const& callback)
{
	user_callbacks.push_back(callback);
	return static_cast<unsigned>(user_callbacks.size() - 1);
}

void Window::RemoveEventCallback(unsigned index)
{
	user_callbacks.at(index) = nullptr;
}

void Window::RequestForClose(bool close)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSetWindowShouldClose(window, close);
}

bool Window::IsRequestedForClose() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	return glfwWindowShouldClose(window);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSwapBuffers(window);
}

void Window::SetPosition(int x, int y)
{
	SetPosition(Ivec2(x, y));
}

void Window::SetPosition(Ivec2 pos)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSetWindowPos(window, pos.x, pos.y);
}

void Window::SetSize(int width, int height)
{
	SetSize(Ivec2(width, height));
}

void Window::SetSize(Ivec2 size)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSetWindowSize(window, size.x, size.y);
}

void Window::SetTitle(char const* title)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSetWindowTitle(window, title);
	this->title = title;
}

Ivec2 Window::GetPosition() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	Ivec2 result;
	glfwGetWindowPos(window, &result.x, &result.y);
	return result;
}

Ivec2 Window::GetSize() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	Ivec2 result;
	glfwGetWindowSize(window, &result.x, &result.y);
	return result;
}

stl::string const& Window::GetTitle() const
{
	return title;
}

void Window::SetResizeAspectRatioLock(int x, int y)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	glfwSetWindowAspectRatio(window, x, y);
}

bool Window::IsKeyPressed(KeyCode keycode) const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	return glfwGetKey(window, keycode) == GLFW_PRESS;
}

bool Window::IsMouseButtonPressed(MouseButton button) const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

Fvec2 Window::GetCursorPosition() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return { x, y };
}

bool Window::IsFocused() const
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	return glfwGetWindowAttrib(window, GLFW_FOCUSED);
}

void Window::SetFullscreenMonitor(int monitor, Ivec2 size)
{
	GLFWwindow* window = static_cast<GLFWwindow*>(nativeptr);
	this->monitor = monitor;
	if (monitor == -1) {
		glfwSetWindowMonitor(window, nullptr, 0, 0, size.x != -1 ? size.x : 1280, size.y != -1 ? size.y : 720, GLFW_DONT_CARE);
		return;
	}

	MonitorsInfo info = MonitorsInfo::Retrieve();
	auto& m = info.Monitors[monitor];
	glfwSetWindowMonitor(window, static_cast<GLFWmonitor*>(m.Resptr), 0, 0,
		size.x != -1 ? size.x : m.Resolution.x, size.y != -1 ? size.y : m.Resolution.y, GLFW_DONT_CARE);
}

int Window::GetFullscreenMonitor() const
{
	return monitor;
}

}