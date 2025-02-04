/* -*-LIC_BEGIN-*- */
/*
*
* MaGE rendering framework
* Emmanuel Chaumont Copyright (c) 2013-2025
*
* This file is part of MaGE.
*
*    MaGE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    MaGE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with MaGE.  If not, see <http://www.gnu.org/licenses/>.
*
*/
/* -*-LIC_END-*- */

#include <string>
#include <unordered_map>
#include <map>
#include <list>

#include "animations.h"
#include "animationssystem.h"
#include "scenenode.h"
#include "entity.h"
#include "entitygraph.h"
#include "aspects.h"
#include "ecshelpers.h"
#include "exceptions.h"
#include "trianglemeshe.h"
#include "shader.h"
#include "tvector.h"
#include "matrix.h"
#include "quaternion.h"
#include "timecontrol.h"

using namespace mage;
using namespace mage::core;

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

void AnimationsSystem::compute_node_animationresult_matrix(const NodeAnimation& p_node, double p_current_tick, core::maths::Matrix& p_out_matrix) const
{
	//////////////////// translations interpolation

	maths::Matrix translation;
	translation.identity();

	if (p_node.position_keys.size() > 0)
	{
		maths::Real4Vector v_interpolated;

		//v_interpolated = p_node.position_keys[p_node.position_keys.size() - 1].value;

		if (p_node.position_keys.size() < 2)
		{
			v_interpolated = p_node.position_keys[0].value;
		}
		else
		{
			if (p_current_tick < p_node.position_keys[0].time_tick)
			{
				v_interpolated = p_node.position_keys[0].value;				
			}
			else if (p_current_tick >= p_node.position_keys[p_node.position_keys.size() - 1].time_tick)
			{
				v_interpolated = p_node.position_keys[p_node.position_keys.size() - 1].value;				
			}
			else
			{
				for (size_t i = 0; i < p_node.position_keys.size() - 1; i++)
				{
					if (p_node.position_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.position_keys[i + 1].time_tick)
					{
						const VectorKey kA{ p_node.position_keys[i] };
						const VectorKey kB{ p_node.position_keys[i + 1] };
						const double blend { (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick) };
						v_interpolated = maths::Vector<double, 4>::lerp(kA.value, kB.value, blend);
						break;
					}
				}
			}
		}


		translation.translation(v_interpolated);
	}

	//////////////////// rotations interpolation

	maths::Matrix rotation;
	rotation.identity();

	if (p_node.rotations_keys.size() > 0)
	{
		maths::Matrix rot_interpolated;

		if (p_node.rotations_keys.size() < 2)
		{
			p_node.rotations_keys[0].value.rotationMatFrom(rot_interpolated);

		}
		else
		{
			if (p_current_tick < p_node.rotations_keys[0].time_tick)
			{
				maths::Quaternion q_interpolated{ p_node.rotations_keys[0].value };
				q_interpolated.rotationMatFrom(rot_interpolated);
			}
			else if (p_current_tick >= p_node.rotations_keys[p_node.rotations_keys.size() - 1].time_tick)
			{
				maths::Quaternion q_interpolated{ p_node.rotations_keys[p_node.rotations_keys.size() - 1].value };
				q_interpolated.rotationMatFrom(rot_interpolated);
			}
			else
			{
				for (size_t i = 0; i < p_node.rotations_keys.size() - 1; i++)
				{
					if (p_node.rotations_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.rotations_keys[i + 1].time_tick)
					{
						const QuaternionKey kA{ p_node.rotations_keys[i] };
						const QuaternionKey kB{ p_node.rotations_keys[i + 1] };

						const double blend{ (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick) };

						maths::Quaternion q_interpolated{ maths::Quaternion::lerp(kA.value, kB.value, blend) };
						q_interpolated.rotationMatFrom(rot_interpolated);
						break;
					}

				}
			}
		}

		rotation = rot_interpolated;
	}

	//////////////////// scaling interpolation

	maths::Matrix scaling;
	scaling.identity();

	if (p_node.scaling_keys.size() > 0)
	{
		maths::Real4Vector v_interpolated;

		if (p_node.scaling_keys.size() < 2)
		{
			v_interpolated = p_node.scaling_keys[0].value;
		}
		else
		{
			if (p_current_tick < p_node.scaling_keys[0].time_tick)
			{
				v_interpolated = p_node.scaling_keys[0].value;
			}
			else if (p_current_tick >= p_node.scaling_keys[p_node.scaling_keys.size() - 1].time_tick)
			{
				v_interpolated = p_node.scaling_keys[p_node.scaling_keys.size() - 1].value;
			}
			else
			{
				for (size_t i = 0; i < p_node.scaling_keys.size() - 1; i++)
				{
					if (p_node.scaling_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.scaling_keys[i + 1].time_tick)
					{
						const VectorKey kA{ p_node.scaling_keys[i] };
						const VectorKey kB{ p_node.scaling_keys[i + 1] };

						const double blend{ (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick) };
						v_interpolated = maths::Vector<double, 4>::lerp(kA.value, kB.value, blend);

						break;
					}
				}
			}
		}

		scaling.scale(v_interpolated);
	}

	p_out_matrix = scaling * rotation * translation;
}

bool AnimationsSystem::animation_step(core::TimeMark& p_tmk, const AnimationKeys& p_animationkeys, std::map<std::string, SceneNode>& p_nodes)
{
	bool status = false;

	const long tms = { p_tmk.computeTimeMs() };

	const double nb_seconds{ (double)tms / 1000.0 };
	double nb_ticks = p_animationkeys.ticks_per_seconds * nb_seconds;

	const double duration_ticks{ p_animationkeys.duration_ticks };

	if (nb_ticks < duration_ticks)
	{
		// animation continue

		for (const auto& e : p_animationkeys.channels)
		{
			maths::Matrix bone_locale_transform;
			compute_node_animationresult_matrix(e.second, nb_ticks, bone_locale_transform);

			if (p_nodes.count(e.second.node_name))
			{
				p_nodes.at(e.second.node_name).locale_transform = bone_locale_transform;
			}
			else
			{
				_EXCEPTION("invalid node name : " + e.second.node_name);
			}
		}
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
			if (p_entity->hasAspect(mage::core::resourcesAspect::id))
			{
				const ComponentContainer& resource_components{ p_entity->aspectAccess(mage::core::resourcesAspect::id)};

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
							auto& animationsList{ p_animation_components.getComponent<std::list<std::pair<std::string,AnimationKeys>>>("eg.std.animationsList")->getPurpose() };

							auto& animationsTimeMark{ p_animation_components.getComponent<core::TimeMark>("eg.std.animationsTimeMark")->getPurpose() };

							//auto& currentAnimationId{ p_animation_components.getComponent<std::string>("eg.std.currentAnimationId")->getPurpose() };

							auto& currentAnimationTicksDuration{ p_animation_components.getComponent<double>("eg.std.currentAnimationTicksDuration")->getPurpose() };
							auto& currentAnimationSecondsDuration{ p_animation_components.getComponent<double>("eg.std.currentAnimationSecondsDuration")->getPurpose() };

							auto& currentAnimationTicksProgress{ p_animation_components.getComponent<double>("eg.std.currentAnimationTicksProgress")->getPurpose() };
							auto& currentAnimationSecondsProgress{ p_animation_components.getComponent<double>("eg.std.currentAnimationSecondsProgress")->getPurpose() };

							
							if (0 == animationsList.size())
							{
								if (animationIdList.size() > 0)
								{			
									const auto& animationId{ animationIdList.front() };

									///////////////////////////////////////////////////////////////////
									// manage transition animation

									const std::string prev_anim_id{ meshe.getPreviousAnimation() };
									if (prev_anim_id != "")
									{
										const auto& animationKeysList{ meshe.getAnimationsKeys() };

										if (animationKeysList.count(prev_anim_id))
										{
											if (animationKeysList.count(animationId))
											{
												const AnimationKeys& prev_anim{ animationKeysList.at(prev_anim_id) };
												const AnimationKeys& next_anim{ animationKeysList.at(animationId) };

												// compute and push transition animation here

												AnimationKeys transition_animation;
												transition_animation.is_transition = true;

												transition_animation.ticks_per_seconds = 30;
												transition_animation.duration_ticks = 5;

												transition_animation.name = "transition";

												for (auto& e : prev_anim.channels)
												{
													if (next_anim.channels.count(e.second.node_name))
													{
														const NodeAnimation next_anim_node{ next_anim.channels.at(e.second.node_name) };
														const NodeAnimation prev_anim_node{ e.second };

														NodeAnimation transition_node_anim;
														transition_node_anim.node_name = e.second.node_name;

														transition_node_anim.position_keys.push_back(prev_anim_node.position_keys[prev_anim_node.position_keys.size() - 1]);
														transition_node_anim.position_keys.push_back(next_anim_node.position_keys[0]);

														transition_node_anim.position_keys[0].time_tick = 0;
														transition_node_anim.position_keys[1].time_tick = transition_animation.duration_ticks;


														transition_node_anim.rotations_keys.push_back(prev_anim_node.rotations_keys[prev_anim_node.rotations_keys.size() - 1]);
														transition_node_anim.rotations_keys.push_back(next_anim_node.rotations_keys[0]);

														transition_node_anim.rotations_keys[0].time_tick = 0;
														transition_node_anim.rotations_keys[1].time_tick = transition_animation.duration_ticks;


														transition_node_anim.scaling_keys.push_back(prev_anim_node.scaling_keys[prev_anim_node.scaling_keys.size() - 1]);
														transition_node_anim.scaling_keys.push_back(next_anim_node.scaling_keys[0]);

														transition_node_anim.scaling_keys[0].time_tick = 0;
														transition_node_anim.scaling_keys[1].time_tick = transition_animation.duration_ticks;

														transition_animation.channels[e.second.node_name] = transition_node_anim;
													}
												}

												animationsList.push_back(std::make_pair(transition_animation.name, transition_animation));
											}
											else
											{
												_EXCEPTION("unknown animation : " + animationId);
											}
										}
										else
										{
											_EXCEPTION("unknown prev animation : " + prev_anim_id);
										}
									}									
									//////////////////////////////////////////////////////////////////
									// take next input anim id, setup and push it

									
									const auto& animationKeysList{ meshe.getAnimationsKeys() };

									if (animationKeysList.count(animationId))
									{
										const AnimationKeys& next_animation{ animationKeysList.at(animationId) };
										animationsList.push_back(std::make_pair(animationId, next_animation));
									}
									else
									{
										_EXCEPTION("unknown animation : " + animationId);
									}
								}
							}
							else
							{
								// roll and play anims in animationsList

								auto& currentAnimationId{ p_animation_components.getComponent<std::string>("eg.std.currentAnimationId")->getPurpose() };
								auto& currentAnimationKey{ p_animation_components.getComponent<AnimationKeys>("eg.std.currentAnimation")->getPurpose() };
								
								if ("" == currentAnimationId)
								{
									// next animation begins

									const auto& animation{ animationsList.front() };

									const std::string& animationId{ animation.first };
									const AnimationKeys& animationkeys{ animation.second };

									currentAnimationId = animationId;
									currentAnimationKey = animationkeys;

									currentAnimationTicksDuration = animationkeys.duration_ticks;
									currentAnimationSecondsDuration = currentAnimationTicksDuration / animationkeys.ticks_per_seconds;

									animationsTimeMark.reset();

									if (!animationkeys.is_transition)
									{
										for (const auto& call : m_callbacks)
										{
											call(AnimationSystemEvent::ANIMATION_START, p_entity->getId(), animationId);
										}
									}
								}

								const long tms = { animationsTimeMark.computeTimeMs() };
								const double nb_seconds{ (double)tms / 1000.0 };
								currentAnimationSecondsProgress = nb_seconds;

								double nb_ticks = currentAnimationKey.ticks_per_seconds * nb_seconds;
								currentAnimationTicksProgress = nb_ticks;

								bool animation_ends{ animation_step(animationsTimeMark, currentAnimationKey, meshe.sceneNodesAccess()) };

								if (animation_ends)
								{
									// this animation ended
									if (!currentAnimationKey.is_transition)
									{
										animationIdList.pop_front();

										meshe.setPreviousAnimation(currentAnimationId);
										for (const auto& call : m_callbacks)
										{
											call(AnimationSystemEvent::ANIMATION_END, p_entity->getId(), currentAnimationId);
										}
									}
									
									animationsList.pop_front();

									currentAnimationId = "";
									currentAnimationTicksDuration = 0;
									currentAnimationSecondsDuration = 0;
									currentAnimationSecondsProgress = 0;
									currentAnimationTicksProgress = 0;
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

	mage::helpers::extractAspectsTopDown<mage::core::animationsAspect>(m_entitygraph, forEachAnimationAspect);

}
