
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

#include "entitygraph.h"
#include "entity.h"
#include "aspectslist.h"

using namespace renderMe;

int main( int argc, char* argv[] )
{    
	std::cout << "ECS tests\n";

	core::Entitygraph eg;
		
	std::cout << "eg has root : " << eg.hasRoot() << "\n";
	auto& root_node{ eg.makeRoot("root")};

	const auto root_entity{ root_node.data() };	
	root_entity->makeAspect(core::renderingAspect);

	{
		// write component in an entity/aspect

		auto& aspect{ root_entity->aspectAccess(core::renderingAspect) };
		aspect.addComponent<int>("width", 640);
	}

	{
		// read component component in an entity/aspect

		auto& aspect{ root_entity->aspectAccess(core::renderingAspect) };
		const auto width{ aspect.getComponent<int>("width")->getPurpose() };
		std::cout << "width = " << width << "\n";
	}
	
	std::cout << "eg has root now : " << eg.hasRoot() << "\n";

	
	auto& ent1_node{ eg.add(root_node, "ent1") };
	auto& ent2_node{ eg.add(root_node, "ent2") };

	auto& ent11_node{ eg.add(ent1_node, "ent11") };
	auto& ent111_node{ eg.add(ent11_node, "ent111") };
	


	/*
	core::Entity ent1("ent1");
	core::Entity ent2("ent2");

	auto& ent1_node{ *(root_node.insert(&ent1)) };
	auto& ent2_node{ *(root_node.insert(&ent2)) };

	core::Entity ent11("ent11");
	auto& ent11_node{ *ent1_node.insert(&ent11) };

	core::Entity ent111("ent111");
	auto& ent111_node{ *ent11_node.insert(&ent111) };
	*/



	// root to leaf browsing
	for (auto it = eg.preBegin(); it != eg.preEnd(); ++it)
	{
		const auto currId{ it->data()->getId() };
		std::cout << currId << "\n";
	}
	std::cout << "\n";

	// leaf to root browsing
	for (auto it = eg.postBegin(); it != eg.postEnd(); ++it)
	{
		const auto currId{ it->data()->getId() };
		std::cout << currId << "\n";
	}
	std::cout << "\n";


	


    return 0;
}
