#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <iostream>

int main()
{
	MayaImat3 mat = {
		10, 20, 30,
		40, 50, 60,
		70, 80, 90
	};

	MayaIvec3 v = { 5, 6, 7 };

	auto res = mat * v;
	std::cout << res.x << res.y << res.z << '\n';
}