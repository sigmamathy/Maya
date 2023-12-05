#include <Maya/core.hpp>
#include <Maya/math.hpp>
#include <iostream>

int main()
{
	MayaIvec2 v = { 5, 6 };
	MayaIvec2 v2 = { 7, 9 };
	auto x = MayaConcat(v, v2);
}