
/* -*-LIC_BEGIN-*- */
/*
*
* renderMe grafx framework
* Emmanuel Chaumont Copyright (c) 2013-2023
*
* This file is part of renderMe.
*
*    renderMe is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    renderMe is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with renderMe.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <iostream>
#include "tvector.h"

using namespace renderMe::core;
using namespace renderMe::core::maths;

int main( int argc, char* argv[] )
{    
	std::cout << "Maths test !\n";

	Vector vdefault;
	std::cout << "default vector : " << vdefault.dump() << "\n";

	Vector<int> vint4(1, 2, 3, 4);
	std::cout << "4 int vector : " << vint4.dump() << "\n";

	Vector<int, 2> vint2;
	std::cout << "2 int vector : " << vint2.dump() << "\n";

	Vector<float, 3> vfloat3(10.0f, 20.0f, 30.0f);
	std::cout << "3 float vector : " << vfloat3.dump() << "\n";

	Vector<double, 2> vdouble2(3.14, 1.57);
	std::cout << "2 double vector : " << vdouble2.dump() << "\n";

	std::cout << vdouble2[1] << "\n";
	vdouble2[1] = 9.81;
	std::cout << vdouble2[1] << "\n";


	Vector<double, 2> vunit(1.0, 1.0);	
	std::cout << "vunit length = " << vunit.length() << "\n";
	vunit.normalize();
	std::cout << "vunit length after normalization = " << vunit.length() << "\n";
	std::cout << "vunit vector (normalized) : " << vunit.dump() << "\n";

	vint4.scale(10);
	std::cout << "4 int vector scaled: " << vint4.dump() << "\n";

	Vector<float, 3> vl0(1.0f, 1.0f, 1.0f);
	Vector<float, 3> vl1(2.0f, 2.0f, 2.0f);

	const auto vlerped{ Vector<float, 3>::lerp(vl0, vl1, 0.43f) };
	std::cout << "lerp result: " << vlerped.dump() << "\n";

	Vector<float, 3> vA(1.0f, -1.0f, 0.5f);
	Vector<float, 3> vB(-1.0f, 2.0f, 2.0f);

	const auto vcp{ Vector<float, 3>::crossProduct(vA, vB) };
	std::cout << "cross-product result: " << vcp.dump() << "\n";

	const float dp{ vA * vB };
	std::cout << "dot-product = " << dp << "\n";

	const auto vSum{ vA + vB };
	std::cout << "vector sum = " << vSum.dump() << "\n";

    return 0;
}
