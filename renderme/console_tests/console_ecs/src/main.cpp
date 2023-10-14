
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
#include "aspects.h"

using namespace renderMe;

int main( int argc, char* argv[] )
{    
	std::cout << "ECS tests\n";

	///// basic tree construction
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	{

		std::cout << "**** Entitygraph test\n\n";

		core::Entitygraph eg;

		std::cout << "eg has root : " << eg.hasRoot() << "\n";
		auto& root_node{ eg.makeRoot("root") };

		const auto root_entity{ root_node.data() };
		root_entity->makeAspect(core::renderingAspect::id);

		{
			// write component in an entity/aspect

			auto& aspect{ root_entity->aspectAccess(core::renderingAspect::id) };
			aspect.addComponent<int>("width", 640);
		}

		{
			// read component component in an entity/aspect

			auto& aspect{ root_entity->aspectAccess(core::renderingAspect::id) };
			const auto width{ aspect.getComponent<int>("width")->getPurpose() };
			std::cout << "width = " << width << "\n";
		}

		std::cout << "eg has root now : " << eg.hasRoot() << "\n";


		auto& ent1_node{ eg.add(root_node, "ent1") };
		auto& ent2_node{ eg.add(root_node, "ent2") };

		eg.add(ent1_node, "ent11");

		eg.add(eg.node("ent11"), "ent111"); // another way to insert a node from its parent



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
	}


	//// systems
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	{

		std::cout << "**** Systems test\n\n";

		core::Entitygraph eg;

		{

			auto& root_window_node{ eg.makeRoot("appwindow") };


			const auto root_window_entity{ root_window_node.data() };
			root_window_entity->makeAspect(core::renderingAspect::id);

			auto& rendering_aspect{ root_window_entity->aspectAccess(core::renderingAspect::id) };

			rendering_aspect.addComponent<core::renderingAspect::renderingTarget>("renderingTarget", core::renderingAspect::renderingTarget::WINDOW_TARGET);
			rendering_aspect.addComponent<bool>("fullscreen", false);
			rendering_aspect.addComponent<int>("windowWidth", 800);
			rendering_aspect.addComponent<int>("windowHeight", 600);
			rendering_aspect.addComponent<bool>("initialized", false);
		}

		{

			while (1)
			{
				// rendering system

				// root to leaf browsing
				for (auto it = eg.preBegin(); it != eg.preEnd(); ++it)
				{
					const auto current_entity{ it->data() };
					const auto currId{ current_entity->getId() };

					if (current_entity->hasAspect(core::renderingAspect::id))
					{
						const auto& rendering_aspect{ current_entity->aspectAccess(core::renderingAspect::id) };

						auto rendering_target_comp{ rendering_aspect.getComponent<core::renderingAspect::renderingTarget>("renderingTarget") };
						if (rendering_target_comp)
						{
							auto& rendering_target{ rendering_target_comp->getPurpose() };

							if (core::renderingAspect::renderingTarget::WINDOW_TARGET == rendering_target)
							{
								//std::cout << "found a WINDOW_TARGET\n";

								auto& initialized{ rendering_aspect.getComponent<bool>("initialized")->getPurpose() };

								if (!initialized)
								{
									std::cout << "initializing rendering...\n";
									initialized = true;
								}
							}
						}
					}
				}
			}			
		}
	}
    return 0;
}
