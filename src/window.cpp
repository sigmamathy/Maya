#include <Maya/window.hpp>
#include <Maya/deviceinfo.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

std::vector<std::shared_ptr<MayaWindow::DataManager>> MayaWindow::window_instances;

MayaWindow::DataManager::~DataManager()
{
	if (MayaIsLibraryInitialized())
		glfwDestroyWindow(static_cast<GLFWwindow*>(internal_pointer));
}

MayaWindow::MayaWindow(MayaWindowParameters const& param)
{
	Init(param);
}

static void CreateWindowEventCallback(GLFWwindow* window)
{
	using WindowData = MayaWindow::DataManager;

	glfwSetKeyCallback(window,
	[](GLFWwindow* window, int key, int scancode, int act, int modes) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (act != GLFW_REPEAT) {
			MayaKeyEvent e(MayaKeyCode(key), act == GLFW_PRESS);
			for (auto& fn : data.event_listeners)
				fn(e);
		}
	});

	glfwSetMouseButtonCallback(window,
	[](GLFWwindow* window, int button, int act, int modes) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (act != GLFW_REPEAT) {
			MayaMouseEvent e(MayaMouseButton(button), act == GLFW_PRESS);
			for (auto& fn : data.event_listeners)
				fn(e);
		}
	});

	glfwSetCursorPosCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaMouseMovedEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data.event_listeners)
			fn(e);
	});

	glfwSetScrollCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaMouseScrolledEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data.event_listeners)
			fn(e);
	});

	glfwSetWindowFocusCallback(window,
	[](GLFWwindow* window, int focus) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaWindowFocusedEvent e(focus);
		for (auto& fn : data.event_listeners)
			fn(e);
	});

	glfwSetWindowCloseCallback(window,
	[](GLFWwindow* window) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (data.is_exit_on_close)
			data.is_closed = true;
		MayaWindowClosedEvent e;
		for (auto& fn : data.event_listeners)
			fn(e);
	});

	glfwSetWindowSizeCallback(window,
	[](GLFWwindow* window, int width, int height) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaWindowResizedEvent e(MayaIvec2(width, height));
		for (auto& fn : data.event_listeners)
			fn(e);
	});

	glfwSetWindowPosCallback(window,
	[](GLFWwindow* window, int x, int y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaWindowMovedEvent e(MayaIvec2(x, y));
		for (auto& fn : data.event_listeners)
			fn(e);
	});
}

void MayaWindow::Init(MayaWindowParameters const& param)
{
	window_instances.push_back(std::make_shared<DataManager>());
	data_manager = window_instances.back();
	auto& d = *window_instances.back().get();

	glfwWindowHint(GLFW_RESIZABLE,		param.resizable);
	glfwWindowHint(GLFW_DECORATED,		param.decorated);
	glfwWindowHint(GLFW_AUTO_ICONIFY,	param.auto_iconify);
	glfwWindowHint(GLFW_FLOATING,		param.always_on_top);
	glfwWindowHint(GLFW_MAXIMIZED,		param.maximized);

	if (param.monitor < 0)
	{
		int sx = param.size.x, sy = param.size.y;
		if (sx < 0) sx = 1280;
		if (sy < 0) sy = 720;
		d.internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(), nullptr, nullptr);
		d.size = { sx, sy };
	}
	else { // fullscreen
		int sx = param.size.x, sy = param.size.y;
		MayaMonitorsInfo info;
		MayaGetDeviceInfo(info);
		auto m = info.monitors->at(param.monitor);
		if (sx < 0) sx = m.resolution.x;
		if (sy < 0) sy = m.resolution.y;
		d.internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(),
			static_cast<GLFWmonitor*>(m.internal_pointer), nullptr);
		d.size = { sx, sy };
	}

	GLFWwindow* ptr = static_cast<GLFWwindow*>(d.internal_pointer);
	glfwMakeContextCurrent(ptr);
	gladLoadGL();

	d.title = param.title;
	d.is_closed = false;
	d.is_exit_on_close = param.exit_on_close;
	d.is_detached = param.detached;
	int px, py;
	glfwGetWindowPos(ptr, &px, &py);
	if (param.position.x >= 0) px = param.position.x;
	if (param.position.y >= 0) py = param.position.y;
	if (param.position.x >= 0 || param.position.y >= 0) 
		glfwSetWindowPos(ptr, px, py);
	d.position = { px, py };

	glfwSetWindowUserPointer(ptr, &d);
	CreateWindowEventCallback(ptr);
}

void MayaWindow::RunEverything()
{
	float start_t = glfwGetTime();
	for (;;)
	{
		bool exit = true;
		for (auto d: window_instances) {
			if (!d->is_detached && !d->is_closed) {
				exit = false;
				break;
			}
		}
		if (exit)
			break;

		float elapsed = glfwGetTime() - start_t;
		start_t = glfwGetTime();
		MayaUpdateEvent e(elapsed);

		for (auto d: window_instances) {
			if (!d->is_closed) {
				for (auto& fn : d->event_listeners)
					fn(e);
				glfwSwapBuffers(static_cast<GLFWwindow*>(d->internal_pointer));
			}
		}
		glfwPollEvents();
	}
	window_instances.clear();
}

void MayaWindow::Close()
{
	auto s = data_manager.lock();
	s->is_closed = true;
}

bool MayaWindow::isClosed() const
{
	auto s = data_manager.lock();
	return s->is_closed;
}