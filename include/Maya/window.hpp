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

// Handles window creation and management.
// Note that object only holds a weak pointer to the actual window.
// Copying this object does not duplicate itself.
class MayaWindowPtr
{
public:
	// Uninitialized.
	MayaWindowPtr() = default;

	// Uninitialized.
	MayaWindowPtr(std::nullptr_t);

	// Add an event listener to the window to track information.
	template<class Ty> requires std::is_base_of_v<MayaEvent, Ty>
	void AddEventListener(std::function<void(MayaWindowPtr, Ty const&)> const& listener);

	// Execute all initialized window at once (not async).
	// Ended only when all windows are either detached or closed.
	static void RunEverything();

	// Close the current window. This process is irreversible.
	// Not that this function does not necessarily destroy the instance.
	void Close();

	// Check whether the window is closed.
	bool IsClosed() const;

	// Check whether the underlying object is destroyed.
	bool IsReachable() const;

	// Check whether two window pointers refer to the same object.
	bool operator==(MayaWindowPtr wptr) const;

	// Modify window size
	void SetSize(MayaIvec2 size);

	// Modify window title
	void SetTitle(std::string const& title);

	// Modify window position
	void SetPosition(MayaIvec2 position);

	// Retrieve window size
	MayaIvec2 GetSize() const;

	// Retrieve window title
	std::string GetTitle() const;

	// Retrieve window position
	MayaIvec2 GetPosition() const;

public:
	// Window data with RAII management
	struct Reference final
	{
		std::string title;
		MayaIvec2 size, position;
		bool is_closed;
		bool is_exit_on_close, is_detached;
		std::vector<std::function<void(MayaWindowPtr, MayaEvent const&)>> event_listeners;
	// ----------- internal ----------- //
		void* internal_pointer;
	
		// Free allocated memory
		~Reference();
	};

private:
	// Assign callbacks
	void AssignWindowEventCallbackToListeners();

	// A pointer that points to the window instance.
	std::weak_ptr<Reference> refptr;

	friend MayaWindowPtr MayaCreateWindow(MayaWindowParameters const&);
	friend MayaWindowPtr MayaRecreateWindow(MayaWindowPtr wptr, MayaWindowParameters const& param);
};

// Register a default window into the system and returns the desired pointer.
MayaWindowPtr MayaCreateWindow();

// Register a window into the system and returns the desired pointer.
MayaWindowPtr MayaCreateWindow(MayaWindowParameters const& param);

// Reload the window with different initial conditions.
// All resources and event callbacks remains.
MayaWindowPtr MayaRecreateWindow(MayaWindowPtr wptr, MayaWindowParameters const& param);

template<class Ty> requires std::is_base_of_v<MayaEvent, Ty>
void MayaWindowPtr::AddEventListener(std::function<void(MayaWindowPtr, Ty const&)> const& listener)
{
#if MAYA_DEBUG
	if (!IsReachable())
		MAYA_DERR(MAYA_EMPTY_REFERENCE_ERROR,
			"MayaWindowPtr::AddEventListener(std::function<void(MayaWindowPtr, Ty const&)> const&): Window has empty reference.");
#endif
	auto s = refptr.lock();
	s->event_listeners.push_back([=](MayaWindowPtr window, MayaEvent const& e) -> void {
		if constexpr (!std::is_same_v<Ty, MayaEvent>) {
			if (e.GetEventID() == Ty::EventID)
				listener(window, *static_cast<Ty const*>(&e));
		} else {
			listener(window, e);
		}
	});
}