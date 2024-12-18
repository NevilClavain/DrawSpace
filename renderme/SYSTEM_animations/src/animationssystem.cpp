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

#include <string>
#include <unordered_map>
#include <map>

#include "animationssystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "exceptions.h"
#include "trianglemeshe.h"

using namespace renderMe;
using namespace renderMe::core;

AnimationsSystem::AnimationsSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{		
}

void AnimationsSystem::run()
{
	const auto forEachAnimationAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_animation_components)
		{
			const auto animationbones_array_register_index_comp { p_animation_components.getComponent<int>("eg.std.animationbones_array_register_index") };

			if (animationbones_array_register_index_comp)
			{
				const int animationbones_array_register_index{ animationbones_array_register_index_comp->getPurpose() };

				// search for the triangle meshe to animate
				if (p_entity->hasAspect(renderMe::core::resourcesAspect::id))
				{
					const ComponentContainer& resource_components{ p_entity->aspectAccess(renderMe::core::resourcesAspect::id)};

					const auto meshes_list{ resource_components.getComponentsByType<std::pair<std::pair<std::string, std::string>, TriangleMeshe>>() };
					if (meshes_list.size() > 0)
					{
						auto& meshe_descr{ meshes_list.at(0)->getPurpose() };
						TriangleMeshe& meshe{ meshe_descr.second };


						const std::function<void(const std::map<std::string, SceneNode>&, 
												 const SceneNode&, 
												const core::maths::Matrix&)> readBonesHierarchy
						{
							[&](const std::map<std::string, SceneNode>& p_scene_nodes, 
								const SceneNode& p_current_node, 
								const core::maths::Matrix& p_parent_transform)
							{
								const maths::Matrix global_transformation;


								for (auto& id : p_current_node.children)
								{
									SceneNode child = p_scene_nodes.at(id);
									readBonesHierarchy(p_scene_nodes, child, global_transformation);
								}
							}
						};



						
					}
				}
			}
			else
			{
				_EXCEPTION("missing animationbones_array_register_index");
			}
		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::animationsAspect>(m_entitygraph, forEachAnimationAspect);

}
