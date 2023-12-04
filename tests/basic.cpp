#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <iostream>

int main()
{
	MayaIvec2 v;
	v.x = 10;
	v.y = 30;
	std::cout << v[0] << v[1] << '\n';
	return 0;
}