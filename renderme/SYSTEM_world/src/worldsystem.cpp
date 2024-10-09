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
#include "renderingqueue.h"
#include "matrixchain.h"
#include "datacloud.h"

using namespace renderMe;
using namespace renderMe::core;

WorldSystem::WorldSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{
}

void WorldSystem::run()
{
	//////////////////////////////////////////////////////////
	/// I : compute transformations
	//////////////////////////////////////////////////////////

	const auto forEachWorldAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_world_aspect)
		{
			///// compute matrix hierarchy

			auto& entity_worldposition_list { p_world_aspect.getComponentsByType<transform::WorldPosition>() };
			if (0 == entity_worldposition_list.size())
			{
				_EXCEPTION("Entity world aspect : missing world position " + p_entity->getId());
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


					switch (entity_worldposition.composition_operation)
					{
						case transform::WorldPosition::TransformationComposition::TRANSFORMATION_RELATIVE_FROM_PARENT:

							entity_worldposition.global_pos = entity_worldposition.local_pos * parententity_worldposition.global_pos;
							break;

						case transform::WorldPosition::TransformationComposition::TRANSFORMATION_ABSOLUTE:

							entity_worldposition.global_pos = entity_worldposition.local_pos;
							break;

						case transform::WorldPosition::TransformationComposition::TRANSFORMATION_PARENT_PROJECTEDPOS:
							{
								const auto comps{ parent_worldaspect.getComponentsByType<core::maths::Real3Vector>() };
								if (comps.size() > 0)
								{
									const auto screenposition_component{ comps.at(0)->getPurpose() };

									auto updated_local_pos{ entity_worldposition.local_pos };

									updated_local_pos(3, 0) += screenposition_component[0];
									updated_local_pos(3, 1) += screenposition_component[1];

									entity_worldposition.projected_z_neg = (screenposition_component[2] < 0);


									entity_worldposition.global_pos = updated_local_pos;
								}
								else
								{
									_EXCEPTION("TRANSFORMATION_PARENT_PROJECTEDPOS mode require 2D projected pos from parent")
								}
							}
							break;
					}
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

	//////////////////////////////////////////////////////////
	/// II : compute 2D pos (for entity that requires it)
	//////////////////////////////////////////////////////////

	// rebuid hierarchical structure to be browsed recursively

	struct ENode
	{
		std::string					 id;
		std::map<std::string, ENode> children;
	};

	ENode root;
	// build node tree that will be dumped to log
	for (auto it = m_entitygraph.preBegin(); it != m_entitygraph.preEnd(); ++it)
	{
		const renderMe::core::Entity* current_entity{ it->data() };
		const std::string currId{ current_entity->getId() };

		const std::function<void(ENode&, const std::string&, const std::string&)> search
		{
			[&](ENode& p_node, const std::string& p_parentId, const std::string& p_id)
			{
				if (p_node.id == p_parentId)
				{
					ENode child;
					child.id = p_id;
					// found parent
					p_node.children[p_id] = child;
				}

				for (auto& e : p_node.children)
				{
					search(e.second, p_parentId, p_id);
				}
			}
		};

		const auto parent_entity{ current_entity->getParent() };
		if (parent_entity)
		{
			const std::string parentId{ parent_entity->getId() };
			search(root, parentId, currId);
		}
		else
		{
			// create root;
			root.id = currId;
		}
	}

	maths::Matrix current_cam;
	maths::Matrix current_view;
	maths::Matrix current_proj;

	current_cam.identity();
	current_view.identity();

	// set a dummy default perspective
	current_proj.perspective(1.0, 0.5, 1.0, 100000.0);

	const std::function<void(const ENode&, int)> browseHierarchy
	{
		[&](const ENode& p_node, int depth)
		{
			const auto& eg_node { m_entitygraph.node(p_node.id) };

			const core::Entity* curr_entity{ eg_node.data() };

			///////////////
			// find current view and current proj

			if (curr_entity->hasAspect(core::renderingAspect::id))
			{
				const auto& rendering_aspect{ curr_entity->aspectAccess(core::renderingAspect::id) };

				const auto rendering_queues_list{ rendering_aspect.getComponentsByType<rendering::Queue>() };
				if (rendering_queues_list.size() > 0)
				{
					auto& renderingQueue{ rendering_queues_list.at(0)->getPurpose() };

					const std::string current_view_entity_id{ renderingQueue.getCurrentView() };

					if (current_view_entity_id != "")
					{
						auto& viewode{ m_entitygraph.node(current_view_entity_id) };
						const auto view_entity{ viewode.data() };

						// extract cam aspect
						const auto& cam_aspect{ view_entity->aspectAccess(cameraAspect::id) };
						const auto& cam_projs_list{ cam_aspect.getComponentsByType<maths::Matrix>() };

						if (0 == cam_projs_list.size())
						{
							_EXCEPTION("entity view aspect : missing projection definition " + view_entity->getId());
						}
						else
						{
							current_proj = cam_projs_list.at(0)->getPurpose();
						}

						// extract world aspect

						const auto& world_aspect{ view_entity->aspectAccess(worldAspect::id) };
						const auto& worldpositions_list{ world_aspect.getComponentsByType<transform::WorldPosition>() };

						if (0 == worldpositions_list.size())
						{
							_EXCEPTION("entity world aspect : missing world position " + view_entity->getId());
						}
						else
						{
							auto& entity_worldposition{ worldpositions_list.at(0)->getPurpose() };
							current_cam = entity_worldposition.global_pos;
						}
					}

					current_view = current_cam;
					current_view.inverse();					
				}
			}

			if (curr_entity->hasAspect(core::worldAspect::id))
			{
				const auto& world_aspect{ curr_entity->aspectAccess(worldAspect::id) };

				const auto comps{ world_aspect.getComponentsByType<core::maths::Real3Vector>() };
				if (comps.size() > 0)
				{
					const auto& screenposition_component{ comps.at(0) };
					const auto& worldpositions_list{ world_aspect.getComponentsByType<transform::WorldPosition>() };

					if (0 == worldpositions_list.size())
					{
						_EXCEPTION("entity world aspect : missing world position " + curr_entity->getId());
					}
					else
					{
						auto& entity_worldposition{ worldpositions_list.at(0)->getPurpose() };
						maths::Matrix entity_world = entity_worldposition.global_pos;

						maths::Matrix inv;
						inv.identity();
						inv(2, 2) = -1.0;
						const auto final_view{ current_view * inv };

						transform::MatrixChain chain;
						chain.pushMatrix(current_proj);
						chain.pushMatrix(final_view);
						chain.pushMatrix(entity_world);
						chain.buildResult();
						auto final_mat{ chain.getResultTransform() };

						core::maths::Real4Vector point(0, 0, 0, 1);
						core::maths::Real4Vector res_point;

						final_mat.transform(&point, &res_point);

						const auto dataCloud{ renderMe::rendering::Datacloud::getInstance() };
						const auto viewport{ dataCloud->readDataValue<maths::FloatCoords2D>("std.viewport") };


						const double posx{ static_cast<double>(res_point[0] / (res_point[2] + 1.0)) * 0.5 * viewport[0] };
						const double posy{ static_cast<double>(res_point[1] / (res_point[2] + 1.0)) * 0.5 * viewport[1] };

						core::maths::Real3Vector projected_pos(posx, posy, res_point[2]);
						screenposition_component->getPurpose() = projected_pos;
					}
				}
			}	
			///////////////

			// recursive call
			for (auto& e : p_node.children)
			{
				browseHierarchy(e.second, depth + 1);
			}
		}
	};

	browseHierarchy(root, 0);
}
