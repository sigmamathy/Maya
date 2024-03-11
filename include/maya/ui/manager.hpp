#pragma once

#include "../render.hpp"
#include "../window.hpp"

namespace maya::ui
{

class Manager
{
public:

	Manager(Window& window);

	~Manager();

	Manager(Manager const&) = delete;
	Manager& operator=(Manager const&) = delete;

	

private:

	unsigned eid;

};

}