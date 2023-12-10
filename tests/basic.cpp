#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <Maya/deviceinfo.hpp>
#include <iostream>

int main()
{
	MayaInitLibrary();

	MayaCPUInfo info;
	MayaGetDeviceInfo(info);
	std::cout << info.brand << " " << info.number_of_cores << '\n';

	MayaTerminateLibrary();
}