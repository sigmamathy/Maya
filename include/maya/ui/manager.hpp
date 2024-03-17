#pragma once

#include "../render.hpp"
#include "../window.hpp"

namespace maya
{

class UiManager
{
public:

	UiManager(Window& window);

	~UiManager();

	UiManager(UiManager const&) = delete;
	UiManager& operator=(UiManager const&) = delete;

private:

	unsigned eid;
	

};

}