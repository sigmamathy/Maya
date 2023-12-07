#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <Maya/deviceinfo.hpp>
#include <iostream>

int main()
{
	MayaInitLibrary();
	MayaRAMInfo info;
	MayaGetDeviceInfo(info);
	std::cout << info.size << " " << '\n';

	MayaTerminateLibrary();
}