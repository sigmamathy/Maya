#include <Maya/window.hpp>
#include <Maya/deviceinfo.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if MAYA_DEBUG
#define MAYA_TEMP_WCHECK(fn) if (!IsReachable()) MAYA_DERR(MAYA_EMPTY_REFERENCE_ERROR, fn ": Window has empty reference.") 
#else
#define MAYA_TEMP_WCHECK(fn)
#endif

std::vector<std::shared_ptr<MayaWindowPtr::Reference>> MayaInternal_window_instances;

MayaWindowPtr::Reference::~Reference()
{
	if (MayaIsLibraryInitialized())
		glfwDestroyWindow(static_cast<GLFWwindow*>(internal_pointer));
}

MayaWindowPtr::MayaWindowPtr(std::nullptr_t)
{
}

MayaWindowPtr MayaCreateWindow()
{
	MayaWindowParameters param;
	return MayaCreateWindow(param);
}

MayaWindowPtr MayaCreateWindow(MayaWindowParameters const& param)
{
	auto r = std::make_shared<MayaWindowPtr::Reference>();
	MayaInternal_window_instances.push_back(r);
	MayaWindowPtr res;
	res.refptr = r;

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
		r->internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(), nullptr, nullptr);
		r->size = { sx, sy };
	}
	else { // fullscreen
		int sx = param.size.x, sy = param.size.y;
		MayaMonitorsInfo info;
		MayaGetDeviceInfo(info);
		auto m = info.monitors->at(param.monitor);
		if (sx < 0) sx = m.resolution.x;
		if (sy < 0) sy = m.resolution.y;
		r->internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(),
			static_cast<GLFWmonitor*>(m.internal_pointer), nullptr);
		r->size = { sx, sy };
	}

	GLFWwindow* ptr = static_cast<GLFWwindow*>(r->internal_pointer);
	glfwMakeContextCurrent(ptr);
	gladLoadGL();

	r->title = param.title;
	r->is_closed = false;
	r->is_exit_on_close = param.exit_on_close;
	r->is_detached = param.detached;
	int px, py;
	glfwGetWindowPos(ptr, &px, &py);
	if (param.position.x >= 0) px = param.position.x;
	if (param.position.y >= 0) py = param.position.y;
	if (param.position.x >= 0 || param.position.y >= 0) 
		glfwSetWindowPos(ptr, px, py);
	r->position = { px, py };

	glfwSetWindowUserPointer(ptr, &MayaInternal_window_instances.back());
	res.AssignWindowEventCallbackToListeners();
	return res;
}

MayaWindowPtr MayaRecreateWindow(MayaWindowPtr wptr, MayaWindowParameters const& param)
{
#if MAYA_DEBUG
	if (!wptr.IsReachable())
		MAYA_DERR(MAYA_EMPTY_REFERENCE_ERROR,
			"MayaWindowPtr MayaRecreateWindow(MayaWindowPtr, MayaWindowParameters const&): Window has empty reference.");
#endif
	auto r = wptr.refptr.lock();
	GLFWwindow* prev = static_cast<GLFWwindow*>(r->internal_pointer);

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
		r->internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(), nullptr, prev);
		r->size = { sx, sy };
	}
	else { // fullscreen
		int sx = param.size.x, sy = param.size.y;
		MayaMonitorsInfo info;
		MayaGetDeviceInfo(info);
		auto m = info.monitors->at(param.monitor);
		if (sx < 0) sx = m.resolution.x;
		if (sy < 0) sy = m.resolution.y;
		r->internal_pointer = glfwCreateWindow(sx, sy, param.title.c_str(),
			static_cast<GLFWmonitor*>(m.internal_pointer), prev);
		r->size = { sx, sy };
	}

	GLFWwindow* ptr = static_cast<GLFWwindow*>(r->internal_pointer);
	glfwMakeContextCurrent(ptr);
	gladLoadGL();

	r->title = param.title;
	r->is_closed = false;
	r->is_exit_on_close = param.exit_on_close;
	r->is_detached = param.detached;
	int px, py;
	glfwGetWindowPos(ptr, &px, &py);
	if (param.position.x >= 0) px = param.position.x;
	if (param.position.y >= 0) py = param.position.y;
	if (param.position.x >= 0 || param.position.y >= 0)
		glfwSetWindowPos(ptr, px, py);
	r->position = { px, py };

	glfwSetWindowUserPointer(ptr, &MayaInternal_window_instances.back());
	wptr.AssignWindowEventCallbackToListeners();
	glfwDestroyWindow(prev);
	return wptr;
}

void MayaWindowPtr::AssignWindowEventCallbackToListeners()
{
	using Wref = std::shared_ptr<MayaWindowPtr::Reference>;
	GLFWwindow* window = static_cast<GLFWwindow*>(refptr.lock()->internal_pointer);

	glfwSetKeyCallback(window,
	[](GLFWwindow* window, int key, int scancode, int act, int modes) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		if (act != GLFW_REPEAT) {
			MayaKeyEvent e(MayaKeyCode(key), act == GLFW_PRESS);
			for (auto& fn : data->event_listeners)
				fn(wptr, e);
		}
	});

	glfwSetMouseButtonCallback(window,
	[](GLFWwindow* window, int button, int act, int modes) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		if (act != GLFW_REPEAT) {
			MayaMouseEvent e(MayaMouseButton(button), act == GLFW_PRESS);
			for (auto& fn : data->event_listeners)
				fn(wptr, e);
		}
	});

	glfwSetCursorPosCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		MayaMouseMovedEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});

	glfwSetScrollCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		MayaMouseScrolledEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});

	glfwSetWindowFocusCallback(window,
	[](GLFWwindow* window, int focus) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		MayaWindowFocusedEvent e(focus);
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});

	glfwSetWindowCloseCallback(window,
	[](GLFWwindow* window) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		if (data->is_exit_on_close)
			data->is_closed = true;
		MayaWindowClosedEvent e;
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});

	glfwSetWindowSizeCallback(window,
	[](GLFWwindow* window, int width, int height) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		data->size = { width, height };
		MayaWindowResizedEvent e(MayaIvec2(width, height));
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});

	glfwSetWindowPosCallback(window,
	[](GLFWwindow* window, int x, int y) {
		auto& data = *static_cast<Wref*>(glfwGetWindowUserPointer(window));
		MayaWindowPtr wptr;
		wptr.refptr = data;
		data->position = { x, y };
		MayaWindowMovedEvent e(MayaIvec2(x, y));
		for (auto& fn : data->event_listeners)
			fn(wptr, e);
	});
}

void MayaWindowPtr::RunEverything()
{
	float start_t = static_cast<float>(glfwGetTime());
	for (;;)
	{
		bool exit = true;
		for (auto d: MayaInternal_window_instances) {
			if (!d->is_detached && !d->is_closed) {
				exit = false;
				break;
			}
		}
		if (exit)
			break;

		float elapsed = static_cast<float>(glfwGetTime()) - start_t;
		start_t = static_cast<float>(glfwGetTime());
		MayaUpdateEvent e(elapsed);

		for (auto d: MayaInternal_window_instances) {
			if (!d->is_closed) {
				glClear(GL_COLOR_BUFFER_BIT);
				MayaWindowPtr wptr;
				wptr.refptr = d;
				for (auto& fn : d->event_listeners)
					fn(wptr, e);
				glfwSwapBuffers(static_cast<GLFWwindow*>(d->internal_pointer));
			}
		}
		glfwPollEvents();
	}
	MayaInternal_window_instances.clear();
}

void MayaWindowPtr::Close()
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::Close()");
	auto s = refptr.lock();
	s->is_closed = true;
}

bool MayaWindowPtr::IsClosed() const
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::IsClosed()");
	auto s = refptr.lock();
	return s->is_closed;
}

bool MayaWindowPtr::IsReachable() const
{
	return !refptr.expired();
}

bool MayaWindowPtr::operator==(MayaWindowPtr wptr) const
{
	return refptr.lock() == wptr.refptr.lock();
}

void MayaWindowPtr::SetSize(MayaIvec2 size)
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::SetSize(MayaIvec2)");
	auto s = refptr.lock();
	s->size = size;
}

void MayaWindowPtr::SetTitle(std::string const& title)
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::SetTitle(std::string const&)");
	auto s = refptr.lock();
	s->title = title;
}

void MayaWindowPtr::SetPosition(MayaIvec2 position)
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::SetPosition(MayaIvec2)");
	auto s = refptr.lock();
	s->position = position;
}

MayaIvec2 MayaWindowPtr::GetSize() const
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::GetSize()");
	auto s = refptr.lock();
	return s->size;
}

std::string MayaWindowPtr::GetTitle() const
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::GetTitle()");
	auto s = refptr.lock();
	return s->title;
}

MayaIvec2 MayaWindowPtr::GetPosition() const
{
	MAYA_TEMP_WCHECK("MayaWindowPtr::GetPosition()");
	auto s = refptr.lock();
	return s->position;
}
