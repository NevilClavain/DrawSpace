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

#include "worldsystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "worldposition.h"
#include "animatorfunc.h"


using namespace renderMe;
using namespace renderMe::core;

WorldSystem::WorldSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void WorldSystem::run()
{
	const auto forEachWorldAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_world_aspect)
		{
			///// compute matrix hierarchy

			auto& entity_worldposition_list { p_world_aspect.getComponentsByType<transform::WorldPosition>() };
			if (0 == entity_worldposition_list.size())
			{
				_EXCEPTION("Eentity world aspect : missing world position " + p_entity->getId());
			}

			auto& entity_worldposition{ entity_worldposition_list.at(0)->getPurpose()};

			entity_worldposition.local_pos.identity();

			// get parent entity if exists
			const auto parent_entity{ p_entity->getParent() };

			if (parent_entity && parent_entity->hasAspect(worldAspect::id))
			{
				const auto& parent_worldaspect{ parent_entity->aspectAccess(worldAspect::id) };
				const auto& parententity_worldpositions_list{ parent_worldaspect.getComponentsByType<transform::WorldPosition>() };

				if (0 == parententity_worldpositions_list.size())
				{
					_EXCEPTION("Parent entity world aspect : missing world position " + parent_entity->getId());
				}
				else
				{
					auto& parententity_worldposition{ parententity_worldpositions_list.at(0)->getPurpose() };

					///// compute animators -> result stored in local pos

					auto& entity_animators_list{ p_world_aspect.getComponentsByType<transform::Animator>() };
					if (entity_animators_list.size() > 0)
					{
						if (p_entity->hasAspect(core::timeAspect::id))
						{
							const auto& time_aspect{ p_entity->aspectAccess(core::timeAspect::id) };

							for (const auto& animator_comp : entity_animators_list)
							{
								const auto& animator{ animator_comp->getPurpose() };
								animator.func(p_world_aspect, time_aspect, parententity_worldposition, animator.component_keys);
							}
						}
						else
						{
							_EXCEPTION("animator requires a time aspect")
						}
					}

					///////////////////////

					entity_worldposition.global_pos = entity_worldposition.local_pos * parententity_worldposition.global_pos;
				}
			}
			else 
			{
				///// compute animators -> result stored in local pos

				auto& entity_animators_list{ p_world_aspect.getComponentsByType<transform::Animator>() };
				if (entity_animators_list.size() > 0)
				{
					if (p_entity->hasAspect(core::timeAspect::id))
					{
						const auto& time_aspect{ p_entity->aspectAccess(core::timeAspect::id) };

						for (const auto& animator_comp : entity_animators_list)
						{
							const auto& animator{ animator_comp->getPurpose() };

							// no parent -> give WorldPosition with identity
							transform::WorldPosition fake_parent_pos;
							fake_parent_pos.global_pos.identity();
							fake_parent_pos.local_pos.identity();

							animator.func(p_world_aspect, time_aspect, fake_parent_pos, animator.component_keys);
						}
					}
					else
					{
						_EXCEPTION("animator requires a time aspect")
					}
				}

				///////////////////////


				entity_worldposition.global_pos = entity_worldposition.local_pos;
			}

			
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::worldAspect>(m_entitygraph, forEachWorldAspect);
}
