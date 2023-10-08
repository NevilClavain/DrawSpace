
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

	core::Entity root("root");
	root.makeAspect(core::renderingAspect);

	{
		// write component in an entity/aspect

		auto& aspect{ root.aspectAccess(core::renderingAspect) };
		aspect.addComponent<int>("width", 640);
	}

	{
		// read component component in an entity/aspect

		auto& aspect{ root.aspectAccess(core::renderingAspect) };
		const auto width{ aspect.getComponent<int>("width")->getPurpose() };
		std::cout << "width = " << width << "\n";
	}


	core::Entitygraph eg;

	std::cout << "eg has root : " << eg.hasRoot() << "\n";
	eg.makeRoot(&root);

	std::cout << "eg has root now : " << eg.hasRoot() << "\n";



	using enode = st_tree::tree<core::Entity*>::node_type;
	using enode_iterator = enode::iterator;

	st_tree::tree<core::Entity*> entityGraph;
	entityGraph.insert(&root);

	enode& root_node{ entityGraph.root() };

	

	core::Entity ent1("ent1");
	enode& ent1_node{ *(root_node.insert(&ent1)) };

	core::Entity ent2("ent2");
	enode& ent2_node{ *(root_node.insert(&ent2)) };
	

	core::Entity ent11("ent11");
	enode& ent11_node{ *ent1_node.insert(&ent11) };

	core::Entity ent12("ent12");
	ent1_node.insert(&ent12);


	core::Entity ent111("ent111");
	ent11_node.insert(&ent111);


	// dummy tree browse
	for (auto e : entityGraph)
	{
		const auto currId{ e.data()->getId() };
		std::cout << currId << "\n";
	}
	
	std::cout << "\n";

	// leaf to root browsing
	for (auto it = entityGraph.df_post_begin(); it != entityGraph.df_post_end(); ++it)
	{
		const auto currId{ it->data()->getId() };
		std::cout << currId << "\n";
	}

	std::cout << "\n";

	// root to leaf browsing
	for (auto it = entityGraph.df_pre_begin(); it != entityGraph.df_pre_end(); ++it)
	{
		const auto currId{ it->data()->getId() };
		std::cout << currId << "\n";
	}

	


    return 0;
}
