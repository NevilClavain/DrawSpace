
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


    return 0;
}
