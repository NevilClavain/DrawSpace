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
#include <list>

#include "animationssystem.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "exceptions.h"
#include "trianglemeshe.h"
#include "shader.h"
#include "tvector.h"
#include "timecontrol.h"

using namespace renderMe;
using namespace renderMe::core;

AnimationsSystem::AnimationsSystem(Entitygraph& p_entitygraph) : System(p_entitygraph)
{		
}

void send_bones_to_shaders(TriangleMeshe& p_meshe, Shader& p_vertex_shader, int p_animationbones_array_arg_index)
{
	auto& animationBones{ p_meshe.animationBonesAccess() };
	const auto& animationBonesNamesMapping{ p_meshe.getAnimationBonesNamesMapping() };

	const std::string& scene_nodes_root_id{ p_meshe.getSceneRootNodeId() };
	const auto& scene_nodes{ p_meshe.getSceneNodes() };

	const std::function<void(const std::map<std::string, SceneNode>&,
		std::vector<AnimationBone>&,
		const std::unordered_map<std::string, int>&,
		const SceneNode&,
		const core::maths::Matrix&)> update_bones_from_nodes_hierarchy
	{
		[&](const std::map<std::string, SceneNode>& p_scene_nodes,
			std::vector<AnimationBone>& p_animation_bones,
			const std::unordered_map<std::string, int>& p_animation_bones_names_mapping,
			const SceneNode& p_current_node,
			const core::maths::Matrix& p_parent_transform)
		{
			const maths::Matrix locale_node_transform = p_current_node.locale_transform;
			const maths::Matrix global_transformation{ locale_node_transform * p_parent_transform };

			if (p_animation_bones_names_mapping.count(p_current_node.id))
			{
				AnimationBone& animation_bone{ p_animation_bones.at(p_animation_bones_names_mapping.at(p_current_node.id)) };
				const maths::Matrix final_transformation{ animation_bone.offset_matrix * global_transformation };

				animation_bone.final_transformation = final_transformation;
			}

			for (auto& id : p_current_node.children)
			{
				SceneNode child = p_scene_nodes.at(id);
				update_bones_from_nodes_hierarchy(p_scene_nodes, p_animation_bones, p_animation_bones_names_mapping, child, global_transformation);
			}
		}
	};
	
	if (scene_nodes_root_id != "")
	{
		core::maths::Matrix mid;
		mid.identity();
		update_bones_from_nodes_hierarchy(scene_nodes, animationBones, animationBonesNamesMapping, scene_nodes.at(scene_nodes_root_id), mid);
	}

	/////////////////////////////////////////////////////////

	auto& dest_array{ p_vertex_shader.vectorArrayArgumentsAccess().at(p_animationbones_array_arg_index)};
	int dest_vector_index{ 0 };

	for (size_t i = 0; i < animationBones.size(); i++)
	{		
		for (size_t col = 0; col < 3; col++)
		{	
			core::maths::Real4Vector columns;

			columns[0] = animationBones.at(i).final_transformation(0, col);
			columns[1] = animationBones.at(i).final_transformation(1, col);
			columns[2] = animationBones.at(i).final_transformation(2, col);
			columns[3] = animationBones.at(i).final_transformation(3, col);

			dest_array.array[dest_vector_index++] = columns;
		}
	}
}

bool AnimationsSystem::animation_step(core::TimeMark& p_tmk, const AnimationKeys& p_animationkeys)
{
	bool status = false;

	const long tms = { p_tmk.computeTimeMs() };

	const double nb_seconds{ (double)tms / 1000.0 };
	double nb_ticks = p_animationkeys.ticks_per_seconds * nb_seconds;

	const double duration_ticks{ p_animationkeys.duration_ticks };

	if (nb_ticks < duration_ticks)
	{
		// animation continue
	}
	else
	{
		// animation ends
		status = true;
	}

	return status;
}

void AnimationsSystem::run()
{
	const auto forEachAnimationAspect
	{
		[&](Entity* p_entity, const ComponentContainer& p_animation_components)
		{
			// search for resources
			if (p_entity->hasAspect(renderMe::core::resourcesAspect::id))
			{
				const ComponentContainer& resource_components{ p_entity->aspectAccess(renderMe::core::resourcesAspect::id)};

				// search triangle meshe
				const auto meshes_list{ resource_components.getComponentsByType<std::pair<std::pair<std::string, std::string>, TriangleMeshe>>() };

				// search the shaders
				const auto shaders_list{ resource_components.getComponentsByType<std::pair<std::string, Shader>>() };

				if (meshes_list.size() > 0 && shaders_list.size() > 0)
				{
					auto& meshe_comp{ meshes_list.at(0)->getPurpose() };
					TriangleMeshe& meshe{ meshe_comp.second };
					auto& vertex_shader{ shaders_list.at(0)->getPurpose().second };

					if (Shader::State::RENDERERLOADED == vertex_shader.getState() && TriangleMeshe::State::RENDERERLOADED == meshe.getState())
					{
						const auto animationbones_array_arg_index_comp{ p_animation_components.getComponent<int>("eg.std.animationbonesArrayArgIndex") };
						if (animationbones_array_arg_index_comp)
						{
							const int animationbones_array_arg_index{ animationbones_array_arg_index_comp->getPurpose() };

							///////////////////////////////////////////////
							
							auto& animationIdList{ p_animation_components.getComponent<std::list<std::string>>("eg.std.animationsIdList")->getPurpose() };
							auto& currentAnimationId{ p_animation_components.getComponent<std::string>("eg.std.currentAnimationId")->getPurpose() };
							auto& animationsTimeMark{ p_animation_components.getComponent<core::TimeMark>("eg.std.animationsTimeMark")->getPurpose() };

							if (animationIdList.size() > 0)
							{
								const auto& animationId = animationIdList.front();								
								const auto& animationKeysList{ meshe.getAnimationsKeys() };

								if (animationKeysList.count(animationId))
								{
									const AnimationKeys& animationkeys{ animationKeysList.at(animationId) };
									if ("" == currentAnimationId)
									{
										currentAnimationId = animationId;
										animationsTimeMark.reset();
									}

									bool animation_ends{ animation_step(animationsTimeMark, animationkeys) };
									if (animation_ends)
									{
										// this animation ended
										animationIdList.pop_front();

										currentAnimationId = "";
									}
								}
								else
								{
									_EXCEPTION("unknown animation : " + animationId);
								}								
							}

							send_bones_to_shaders(meshe, vertex_shader, animationbones_array_arg_index);
							
							////////////////////////////////////////////////
						}
						else
						{
							_EXCEPTION("missing animationbones_array_arg_index");
						}
					}
				}
			}

		}
	};

	renderMe::helpers::extractAspectsTopDown<renderMe::core::animationsAspect>(m_entitygraph, forEachAnimationAspect);

}
