#pragma once

#include "./math.hpp"
#include "./event.hpp"

// Parameters required to initialize a MayaWindow
struct MayaWindowParameters
{
	// Specify the size of window. -1 stands for default, which.
	// monitor < 0:			1280x720
	// fullscreen:			monitor resolution
	MayaIvec2 size			= {-1, -1};

	// Specify the title of window, could be empty.
	std::string title		= "Maya Application";

	// Specify the position of window. -1 stands for default, which
	// monitor < 0:			default position
	// fullscreen:			no use
	MayaIvec2 position		= {-1, -1};

	// Specify a monitor for fullscreen.
	// The value stands for the priority amongst monitors connected.
	// -1: no fullscreen is required.
	int monitor				= -1;

	// Window creation hints
	bool resizable			= true,
		 decorated			= true,
		 auto_iconify		= true,
		 always_on_top		= false,
		 maximized			= false;

	// Operation handling
	bool exit_on_close		= true,
		 detached			= false;
};


class MayaWindow
{
public:
	// Uninitialized
	MayaWindow() = default;

	// Same functionality as Init
	MayaWindow(MayaWindowParameters const& param);

	// Initialize window object using the given information
	void Init(MayaWindowParameters const& param);

	template<class Ty> requires std::is_base_of_v<MayaEvent, Ty>
	void AddEventListener(std::function<void(Ty const&)> const& listener);

	static void RunEverything();

	void Close();

	bool isClosed() const;

public:
	// Window data with RAII management
	struct DataManager final
	{
		std::string title;
		MayaIvec2 size, position;
		bool is_closed;
		bool is_exit_on_close, is_detached;
		std::vector<std::function<void(MayaEvent const&)>> event_listeners;
	// ----------- internal ----------- //
		void* internal_pointer;
	
		// Free allocated memory
		~DataManager();
	};

private:
	static std::vector<std::shared_ptr<DataManager>> window_instances;

	std::weak_ptr<DataManager> data_manager;
};

template<class Ty> requires std::is_base_of_v<MayaEvent, Ty>
void MayaWindow::AddEventListener(std::function<void(Ty const&)> const& listener)
{
	auto s = data_manager.lock();
	s->event_listeners.push_back([=](MayaEvent const& e) -> void {
		e.Dispatch(listener);
	});
}