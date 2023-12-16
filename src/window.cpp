#include <Maya/window.hpp>
#include <Maya/deviceinfo.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if MAYA_DEBUG
#define MAYA_TEMP_WCHECK(fn) if (!IsReachable()) MAYA_DERR(MAYA_EMPTY_REFERENCE_ERROR, fn ": Window has empty reference.") 
#else
#define MAYA_TEMP_WCHECK(fn)
#endif

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

	glfwSetWindowUserPointer(ptr, &window_instances.back());
	AssignWindowEventCallbackToListeners();
}

MayaWindow::MayaWindow(std::weak_ptr<DataManager> manager)
	: data_manager(manager)
{
}

void MayaWindow::AssignWindowEventCallbackToListeners()
{
	using WindowData = std::shared_ptr<MayaWindow::DataManager>;
	GLFWwindow* window = static_cast<GLFWwindow*>(data_manager.lock()->internal_pointer);

	glfwSetKeyCallback(window,
	[](GLFWwindow* window, int key, int scancode, int act, int modes) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (act != GLFW_REPEAT) {
			MayaKeyEvent e(MayaKeyCode(key), act == GLFW_PRESS);
			for (auto& fn : data->event_listeners)
				fn(MayaWindow(data), e);
		}
	});

	glfwSetMouseButtonCallback(window,
	[](GLFWwindow* window, int button, int act, int modes) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (act != GLFW_REPEAT) {
			MayaMouseEvent e(MayaMouseButton(button), act == GLFW_PRESS);
			for (auto& fn : data->event_listeners)
				fn(MayaWindow(data), e);
		}
	});

	glfwSetCursorPosCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaMouseMovedEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});

	glfwSetScrollCallback(window,
	[](GLFWwindow* window, double x, double y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaMouseScrolledEvent e(MayaIvec2(static_cast<int>(x), static_cast<int>(y)));
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});

	glfwSetWindowFocusCallback(window,
	[](GLFWwindow* window, int focus) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		MayaWindowFocusedEvent e(focus);
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});

	glfwSetWindowCloseCallback(window,
	[](GLFWwindow* window) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		if (data->is_exit_on_close)
			data->is_closed = true;
		MayaWindowClosedEvent e;
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});

	glfwSetWindowSizeCallback(window,
	[](GLFWwindow* window, int width, int height) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->size = { width, height };
		MayaWindowResizedEvent e(MayaIvec2(width, height));
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});

	glfwSetWindowPosCallback(window,
	[](GLFWwindow* window, int x, int y) {
		auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->position = { x, y };
		MayaWindowMovedEvent e(MayaIvec2(x, y));
		for (auto& fn : data->event_listeners)
			fn(MayaWindow(data), e);
	});
}


void MayaWindow::RunEverything()
{
	float start_t = static_cast<float>(glfwGetTime());
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

		float elapsed = static_cast<float>(glfwGetTime()) - start_t;
		start_t = static_cast<float>(glfwGetTime());
		MayaUpdateEvent e(elapsed);

		for (auto d: window_instances) {
			if (!d->is_closed) {
				glClear(GL_COLOR_BUFFER_BIT);
				for (auto& fn : d->event_listeners)
					fn(MayaWindow(d), e);
				glfwSwapBuffers(static_cast<GLFWwindow*>(d->internal_pointer));
			}
		}
		glfwPollEvents();
	}
	window_instances.clear();
}

void MayaWindow::Close()
{
	MAYA_TEMP_WCHECK("MayaWindow::Close()");
	auto s = data_manager.lock();
	s->is_closed = true;
}

bool MayaWindow::IsClosed() const
{
	MAYA_TEMP_WCHECK("MayaWindow::IsClosed()");
	auto s = data_manager.lock();
	return s->is_closed;
}

bool MayaWindow::IsReachable() const
{
	return !data_manager.expired();
}

void MayaWindow::SetSize(MayaIvec2 size)
{
	MAYA_TEMP_WCHECK("MayaWindow::SetSize(MayaIvec2)");
	auto s = data_manager.lock();
	s->size = size;
}

void MayaWindow::SetTitle(std::string const& title)
{
	MAYA_TEMP_WCHECK("MayaWindow::SetTitle(std::string const&)");
	auto s = data_manager.lock();
	s->title = title;
}

void MayaWindow::SetPosition(MayaIvec2 position)
{
	MAYA_TEMP_WCHECK("MayaWindow::SetPosition(MayaIvec2)");
	auto s = data_manager.lock();
	s->position = position;
}

MayaIvec2 MayaWindow::GetSize() const
{
	MAYA_TEMP_WCHECK("MayaWindow::GetSize()");
	auto s = data_manager.lock();
	return s->size;
}

std::string MayaWindow::GetTitle() const
{
	MAYA_TEMP_WCHECK("MayaWindow::GetTitle()");
	auto s = data_manager.lock();
	return s->title;
}

MayaIvec2 MayaWindow::GetPosition() const
{
	MAYA_TEMP_WCHECK("MayaWindow::GetPosition()");
	auto s = data_manager.lock();
	return s->position;
}

// Vertex shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 position;

    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 fragColor;
	uniform vec4 color;

    void main()
    {
        fragColor = color;
    }
)";

static MayaViewport* current_viewport = nullptr;

MayaViewport::MayaViewport(MayaWindow window)
	: window(window), parent(nullptr)
{
	exact_position = { 0, 0 };
	exact_size = window.GetSize();
}

MayaViewport::MayaViewport(MayaViewport* viewport)
	: window(window), parent(viewport)
{
	exact_position = viewport->exact_position;
	exact_size = viewport->exact_size;
}

void MayaViewport::ClearColor(MayaFvec4 color)
{
	UseThisViewport();

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Create the shader program and link the shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Create and bind the vertex array object (VAO)
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the vertex buffer object (VBO) and copy the vertex data
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	float vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 -1.0f, 1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,
		1.0f, -1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Specify the vertex attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram);

	glUniform4f(glGetUniformLocation(shaderProgram, "color"), color.x, color.y, color.z, color.w);

	// Bind the VAO
	glBindVertexArray(vao);

	// Draw the square
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unbind the VAO
	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
}

void MayaViewport::UseThisViewport()
{
	ComputeViewportBounds();
	current_viewport = this;
	glViewport(exact_position.x, exact_position.y, exact_size.x, exact_size.y);
}

MayaIvec2 MayaViewport::GetParentExactPosition() const
{
	return parent ? parent->exact_position : MayaIvec2(0);
}

MayaIvec2 MayaViewport::GetParentExactSize() const
{
	return parent ? parent->exact_size : window.GetSize();
}

MayaFloatingViewport::MayaFloatingViewport(MayaWindow window)
	: MayaViewport(window)
{
	local_position = { 0, 0 };
	local_size = exact_size;
	dock = NoDock;
	align = AlignTopLeft;
}

MayaFloatingViewport::MayaFloatingViewport(MayaViewport* viewport)
	: MayaViewport(viewport)
{
	local_position = { 0, 0 };
	local_size = exact_size;
	dock = NoDock;
	align = AlignTopLeft;
}

static void SwitchBetweenBLAndOtherDirection(MayaFloatingViewport::AlignDirecton align, MayaIvec2 ps,
											MayaIvec2& out, MayaIvec2 inp, MayaIvec2 ins)
{
	switch (align)
	{
	using enum MayaFloatingViewport::AlignDirecton;
		case AlignTopLeft:
			out.x = inp.x;
			out.y = ps.y - inp.y - ins.y;
			break;
		case AlignTopRight:
			out.x = ps.x - inp.x - ins.x;
			out.y = ps.y - inp.y - ins.y;
			break;
		case AlignBottomLeft:
			out.x = inp.x;
			out.y = inp.y;
			break;
		case AlignBottomRight:
			out.x = ps.x - inp.x - ins.x;
			out.y = inp.y;
			break;
	}
}

void MayaFloatingViewport::SetAlignDirection(AlignDirecton dir)
{
	auto const ps = GetParentExactSize();
	MayaIvec2 tmp;
	SwitchBetweenBLAndOtherDirection(align, ps, tmp, local_position, local_size);
	SwitchBetweenBLAndOtherDirection(dir, ps, local_position, tmp, local_size);
	align = dir;
}

MayaFloatingViewport::AlignDirecton MayaFloatingViewport::GetAlignDirection() const
{
	return align;
}

void MayaFloatingViewport::SetPosition(MayaIvec2 position)
{
	dock = NoDock;
	local_position = position;
}

void MayaFloatingViewport::SetSize(MayaIvec2 size)
{
	dock = NoDock;
	local_size = size;
}

void MayaFloatingViewport::SetBounds(MayaIvec4 bounds)
{
	dock = NoDock;
	local_size = { bounds.z, bounds.w };
	local_position = { bounds.x, bounds.y };
	ComputeViewportBounds();
}

MayaIvec2 MayaFloatingViewport::GetPosition() const
{
	return local_position;
}

MayaIvec2 MayaFloatingViewport::GetSize() const
{
	return local_size;
}

MayaIvec4 MayaFloatingViewport::GetBounds() const
{
	return MayaConcat(local_position, local_size);
}

void MayaFloatingViewport::SetDockDirection(DockDirection dir)
{
	dock = dir;
	ComputeViewportBounds();
}

MayaFloatingViewport::DockDirection MayaFloatingViewport::GetDockDirection() const
{
	return dock;
}

void MayaFloatingViewport::SetDockThickness(int thickness)
{
	if (dock == DockLeft || dock == DockRight)
		local_size.x = thickness;
	else if (dock == DockTop || dock == DockBottom)
		local_size.y = thickness;
	ComputeViewportBounds();
}

void MayaFloatingViewport::ComputeViewportBounds()
{
	auto const pp = GetParentExactPosition();
	auto const ps = GetParentExactSize();

	switch (dock) {
		case DockLeft:
			SwitchBetweenBLAndOtherDirection(align, ps, local_position, { 0, 0 }, local_size);
			local_size.y = ps.y;
			break;
		case DockRight:
			SwitchBetweenBLAndOtherDirection(align, ps, local_position, { ps.x - local_size.x, 0 }, local_size);
			local_size.y = ps.y;
			break;
		case DockTop:
			SwitchBetweenBLAndOtherDirection(align, ps, local_position, { 0, ps.y - local_size.y }, local_size);
			local_size.x = ps.x;
			break;
		case DockBottom:
			SwitchBetweenBLAndOtherDirection(align, ps, local_position, { 0, 0 }, local_size);
			local_size.x = ps.x;
			break;
	}

	SwitchBetweenBLAndOtherDirection(align, ps, exact_position, local_position, local_size);
	exact_position = exact_position + pp;
	exact_size = local_size;
}