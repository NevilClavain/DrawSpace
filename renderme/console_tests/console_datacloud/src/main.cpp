
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

#include "datacloud.h"
#include "tvector.h"

using namespace renderMe;
using namespace renderMe::core::maths;

int main( int argc, char* argv[] )
{    
	std::cout << "Datacloud tests\n";

	renderMe::rendering::Datacloud::getInstance()->registerData<Real4Vector>("mycolor");

	{
		const auto mycolorVal{ renderMe::rendering::Datacloud::getInstance()->readDataValue<Real4Vector>("mycolor") };
		std::cout << "mycolor = " << mycolorVal[0] << " " << mycolorVal[1] << " " << mycolorVal[2] << " " << mycolorVal[3] << "\n";
	}

	renderMe::rendering::Datacloud::getInstance()->updateData<Real4Vector>("mycolor", Real4Vector(0.3, 0.2, 0.1, 1.0));

	{
		const auto mycolorVal{ renderMe::rendering::Datacloud::getInstance()->readDataValue<Real4Vector>("mycolor") };
		std::cout << "mycolor = " << mycolorVal[0] << " " << mycolorVal[1] << " " << mycolorVal[2] << " " << mycolorVal[3] << "\n";
	}


    return 0;
}
