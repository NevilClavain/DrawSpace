
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

#include "entity.h"
#include "aspectslist.h"

using namespace renderMe;

int main( int argc, char* argv[] )
{    
	std::cout << "ECS tests\n";

	core::entity ent;

	ent.makeAspect(core::renderingAspect);

	{
		// write component in an entity/aspect

		auto& aspect{ ent.aspectAccess(core::renderingAspect) };
		aspect.addComponent<int>("width", 640);
	}


	{
		// read component component in an entity/aspect

		auto& aspect{ ent.aspectAccess(core::renderingAspect) };
		const auto width{ aspect.getComponent<int>("width")->getPurpose() };
		std::cout << "width = " << width << "\n";
	}


    return 0;
}
