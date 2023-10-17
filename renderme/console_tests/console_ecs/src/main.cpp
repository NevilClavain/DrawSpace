
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
#include "system.h"
#include "sysengine.h"

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
		}

		{
			//////////////////////////////////////////////////////////

			constexpr int myRenderingSystemExecutionSlot{ 0 };

			class MyRenderingSystem : public core::System
			{
			public:

				MyRenderingSystem(core::Entitygraph& p_entitygraph) : System(p_entitygraph)
				{
				}
				~MyRenderingSystem() = default;

				void run()
				{

					// root to leaf browsing
					for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
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

									if (!m_initialized)
									{
										std::cout << "initializing rendering...\n";

										// do initialization stuff here...

										m_initialized = true;
									}
								}
							}
						}						
					}
				}

			private:
				bool	m_initialized{ false };
			};

			MyRenderingSystem myRenderingSystem(eg);

			//////////////////////////////////////////////////////////

			// systems management...

			auto sysEngine{ core::SystemEngine::getInstance() };
			sysEngine->makeSystem<MyRenderingSystem>(myRenderingSystemExecutionSlot, eg);
			
			while (1)
			{
				sysEngine->run();
			}
			
			//////////////////////////////////////////////////////////
		}
	}
    return 0;
}
