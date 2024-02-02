#pragma once

#include "../core.hpp"
#include "../math.hpp"
#include "../renderer.hpp"
#include "../window.hpp"

struct MayaGraphics2D {
	struct SingletonPerWindow;
};

struct MayaGraphics2D::SingletonPerWindow
{
	SingletonPerWindow(MayaWindow& window);
	~SingletonPerWindow();
	SingletonPerWindow(SingletonPerWindow const&) = delete;
	SingletonPerWindow& operator=(SingletonPerWindow const&) = delete;
	MayaWindow* const Window;
};