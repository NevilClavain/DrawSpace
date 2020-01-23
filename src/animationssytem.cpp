/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
*                                                                          
* This file is part of DrawSpace.                                          
*                                                                          
*    DrawSpace is free software: you can redistribute it and/or modify     
*    it under the terms of the GNU General Public License as published by  
*    the Free Software Foundation, either version 3 of the License, or     
*    (at your option) any later version.                                   
*                                                                          
*    DrawSpace is distributed in the hope that it will be useful,          
*    but WITHOUT ANY WARRANTY; without even the implied warranty of        
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
*    GNU General Public License for more details.                          
*                                                                          
*    You should have received a copy of the GNU General Public License     
*    along with DrawSpace.  If not, see <http://www.gnu.org/licenses/>.    
*
*/
/* -*-LIC_END-*- */

#include "animationssystem.h"
#include "transformaspect.h"
#include "mesherenderingaspectimpl.h"
#include "maths.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Core;
using namespace DrawSpace::AspectImplementations;

const dsstring AnimationsSystem::bonesBuffer0Id = "bones_0";
const dsstring AnimationsSystem::bonesBuffer1Id = "bones_1";
const dsstring AnimationsSystem::transitionAnimationDurationId = "transition_animation";
const dsreal   AnimationsSystem::transitionAnimationDurationSeconds = 0.4;

AnimationsSystem::AnimationsSystem(void)
{
}

AnimationsSystem::~AnimationsSystem(void)
{
}

void AnimationsSystem::run(EntityGraph::EntityNodeGraph* p_entitygraph)
{
    p_entitygraph->AcceptSystemRootToLeaf(this);
}

void AnimationsSystem::read_bones_hierarchy(const std::map<dsstring, AnimationsAspect::Node>& p_nodes, 
											std::vector<AnimationsAspect::BoneOutput>& p_bones_output, 
											const std::map<dsstring, int>& p_bones_mapping, 
											AnimationsAspect::Node p_node,
											const DrawSpace::Utils::Matrix& p_parent_transform)
{
	Matrix locale_node_transform = p_node.locale_transform;
	Matrix global_transformation = locale_node_transform * p_parent_transform;

	if (p_bones_mapping.count(p_node.id))
	{
		Matrix final_transformation = p_bones_output.at(p_bones_mapping.at(p_node.id)).offset_matrix * global_transformation;
		p_bones_output.at(p_bones_mapping.at(p_node.id)).final_transformation = final_transformation;
	}

	for (auto& e : p_node.children)
	{
		std::string id = e;
		AnimationsAspect::Node child = p_nodes.at(id);
		read_bones_hierarchy(p_nodes, p_bones_output, p_bones_mapping, child, global_transformation);
	}
}

void AnimationsSystem::compute_node_animationresult_matrix(const AnimationsAspect::NodeAnimation& p_node, dsreal p_current_tick, Utils::Matrix& p_out_matrix) const
{
	//////////////////// translations interpolation

	Utils::Matrix translation;
	translation.Identity();

	bool t_computed = false;
	bool r_computed = false;
	bool s_computed = false;

	
	if (p_node.position_keys.size() > 0)
	{
		Utils::Vector v_interpolated;
		if (p_node.position_keys.size() < 2)
		{
			v_interpolated = p_node.position_keys[0].value;
			t_computed = true;
		}
		else
		{
			if( p_current_tick < p_node.position_keys[0].time_tick )
			{
				v_interpolated = p_node.position_keys[0].value;
				t_computed = true;
			}
			else if (p_current_tick >= p_node.position_keys[p_node.position_keys.size() - 1].time_tick)
			{
				v_interpolated = p_node.position_keys[p_node.position_keys.size() - 1].value;
				t_computed = true;
			}
			else
			{
				for (size_t i = 0; i < p_node.position_keys.size() - 1; i++)
				{
					if (p_node.position_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.position_keys[i + 1].time_tick)
					{
						AnimationsAspect::VectorKey kA = p_node.position_keys[i];
						AnimationsAspect::VectorKey kB = p_node.position_keys[i + 1];

						dsreal blend = (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick);
						v_interpolated = Utils::Vector::Lerp(kA.value, kB.value, blend);
						t_computed = true;
						break;
					}
				}
			}
		}

		if (!t_computed)
		{
			_DSEXCEPTION( "Position interpolation failed !" )
		}

		translation.Translation(v_interpolated);
	}
	
	//////////////////// rotations interpolation

	Utils::Matrix rotation;
	rotation.Identity();
	
	if (p_node.rotations_keys.size() > 0)
	{
		Utils::Matrix rot_interpolated;
		if (p_node.rotations_keys.size() < 2)
		{
			p_node.rotations_keys[0].value.RotationMatFrom(rot_interpolated);
			r_computed = true;
		}
		else
		{
			if (p_current_tick < p_node.rotations_keys[0].time_tick)
			{
				Quaternion q_interpolated = p_node.rotations_keys[0].value;
				q_interpolated.RotationMatFrom(rot_interpolated);

				r_computed = true;
			}
			else if (p_current_tick >= p_node.rotations_keys[p_node.rotations_keys.size() - 1].time_tick)
			{

				Quaternion q_interpolated = p_node.rotations_keys[p_node.rotations_keys.size() - 1].value;
				q_interpolated.RotationMatFrom(rot_interpolated);

				r_computed = true;
			}
			else
			{
				for (size_t i = 0; i < p_node.rotations_keys.size() - 1; i++)
				{
					if (p_node.rotations_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.rotations_keys[i + 1].time_tick)
					{
						AnimationsAspect::QuaternionKey kA = p_node.rotations_keys[i];
						AnimationsAspect::QuaternionKey kB = p_node.rotations_keys[i + 1];

						dsreal blend = (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick);

						Quaternion q_interpolated = Utils::Quaternion::Lerp(kA.value, kB.value, blend);
						q_interpolated.RotationMatFrom(rot_interpolated);
						r_computed = true;
						break;
					}

				}
			}
		}

		if (!r_computed)
		{
			_DSEXCEPTION("Rotation interpolation failed !")
		}

		rotation = rot_interpolated;
	}
	
	//////////////////// scaling interpolation

	Utils::Matrix scaling;
	scaling.Identity();
	
	if (p_node.scaling_keys.size() > 0)
	{
		Utils::Vector v_interpolated;
		if (p_node.scaling_keys.size() < 2)
		{
			v_interpolated = p_node.scaling_keys[0].value;
			s_computed = true;

		}
		else
		{
			if (p_current_tick < p_node.scaling_keys[0].time_tick)
			{
				s_computed = true;

				v_interpolated = p_node.scaling_keys[0].value;
			}
			else if (p_current_tick >= p_node.scaling_keys[p_node.scaling_keys.size() - 1].time_tick)
			{
				s_computed = true;

				v_interpolated = p_node.scaling_keys[p_node.scaling_keys.size() - 1].value;

			}
			else
			{
				for (size_t i = 0; i < p_node.scaling_keys.size() - 1; i++)
				{
					if (p_node.scaling_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.scaling_keys[i + 1].time_tick)
					{
						AnimationsAspect::VectorKey kA = p_node.scaling_keys[i];
						AnimationsAspect::VectorKey kB = p_node.scaling_keys[i + 1];

						dsreal blend = (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick);
						v_interpolated = Utils::Vector::Lerp(kA.value, kB.value, blend);

						s_computed = true;
						break;
					}

				}
			}
		}

		if (!s_computed)
		{
			_DSEXCEPTION("Scaling interpolation failed !")
		}

		scaling.Scale(v_interpolated);
	}

	///////////////////////
	

	p_out_matrix = scaling * rotation * translation;
}

bool AnimationsSystem::animation_step(Entity* p_entity, const dsstring& p_animation_id, const AnimationsAspect::AnimationRoot& p_animation,
										AnimationsAspect* p_anims_aspect, 
										std::map<dsstring, AnimationsAspect::Node>& p_nodes)
{
	bool status = false;
	TimeAspect::TimeMark& tmk = p_anims_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose();

	long tms = tmk.ComputeTimeMs();
	dsreal nb_seconds = (dsreal)tms / 1000.0;
	dsreal nb_ticks = p_anims_aspect->GetComponent<long>("current_animation_ticks_per_seconds")->getPurpose() * nb_seconds;

	dsreal current_animation_ticks_duration = p_anims_aspect->GetComponent<dsreal>("current_animation_ticks_duration")->getPurpose();
	if (nb_ticks < current_animation_ticks_duration)
	{
		p_anims_aspect->GetComponent<dsreal>("current_animation_seconds_progress")->getPurpose() = nb_seconds;
		p_anims_aspect->GetComponent<dsreal>("current_animation_ticks_progress")->getPurpose() = nb_ticks;

		for (auto& e : p_animation.channels)
		{
			Utils::Matrix bone_locale_transform;
			compute_node_animationresult_matrix(e.second, nb_ticks, bone_locale_transform);

			if (p_nodes.count(e.second.node_name))
			{
				p_nodes.at(e.second.node_name).locale_transform = bone_locale_transform;
			}
			else
			{
				_DSEXCEPTION("invalid node name");
			}
		}
	}
	else
	{
		// animation end
		status = true;

		// animation end event
		for (auto& e : m_evt_handlers)
		{
			(*e)(p_entity, ANIMATION_END, p_animation_id);
		}
	}

	return status;
}

void AnimationsSystem::run_animations_pool(Entity* p_entity, DrawSpace::Aspect::AnimationsAspect::AnimationsPool& p_animations_pool, DrawSpace::Aspect::AnimationsAspect* p_anims_aspect,
											DrawSpace::Aspect::TimeAspect* p_time_aspect,
											std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Node>& p_nodes)
{
	if(p_animations_pool.size() > 0)
	{		
		const auto& animation = p_animations_pool.front();
		dsstring anim_id = animation.first;

		auto& current_animation_name = p_anims_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose();
		if ("" == current_animation_name)
		{
			// setup current animation infos components...

			p_anims_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose() = anim_id;
			p_anims_aspect->GetComponent<long>("current_animation_ticks_per_seconds")->getPurpose() = animation.second.ticksPerSeconds;

			p_anims_aspect->GetComponent<dsreal>("current_animation_ticks_progress")->getPurpose() = 0;
			p_anims_aspect->GetComponent<dsreal>("current_animation_seconds_progress")->getPurpose() = 0;

			p_anims_aspect->GetComponent<dsreal>("current_animation_ticks_duration")->getPurpose() = animation.second.duration;
			p_anims_aspect->GetComponent<dsreal>("current_animation_seconds_duration")->getPurpose() = animation.second.duration / animation.second.ticksPerSeconds;

			p_anims_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose() = p_time_aspect->TimeMarkFactory();
			p_anims_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose().Reset();
		}
		
		if (true == animation_step(p_entity, animation.first, animation.second, p_anims_aspect, p_nodes))
		{
			// record id of this finished animation (if not transition animation)
			dsstring& last_animation_name = p_anims_aspect->GetComponent<dsstring>("last_animation_name")->getPurpose();
			last_animation_name = anim_id;

			// this animation ended
			p_animations_pool.pop_front();

			// reset current_animation_name component to force setup next animation infos components next time of AnimationsSystem::run_animations_pool() call
			current_animation_name = "";
		}
	}
}

void AnimationsSystem::VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity)
{
    AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
    if (anims_aspect)
    {
		insert_transition_animation(p_entity, anims_aspect);

		TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
		if (NULL == transform_aspect)
		{
			_DSEXCEPTION("Entity must have transformation aspect");
		}
		TimeAspect* time_aspect = transform_aspect->GetTimeAspectRef();

		auto& bones = anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose();
		auto& animations_pool = anims_aspect->GetComponent<AnimationsAspect::AnimationsPool>("animations_pool")->getPurpose();

		run_animations_pool(p_entity, animations_pool, anims_aspect, time_aspect, bones);

		///////////////////////////////////////////////////////////////////////////////////////////////
		//// get some eventually forced bones position

		apply_forced_bones_pos(anims_aspect);

		///////////////////////////////////////////////////////////////////////////////////////////////
		///// directly apply last key of an animation

		apply_animation_last_key(anims_aspect);

		///////////////////////////////////////////////////////////////////////////////////////////////
		///// Push bones matrix to shader

		RenderingAspect* rendering_aspect = p_entity->GetAspect <RenderingAspect>();
		if (!rendering_aspect)
		{
			_DSEXCEPTION("An entity with AnimationsAspect must also have a RenderingAspect");
		}

		send_bones_to_shaders(anims_aspect, rendering_aspect);
    }
}

void AnimationsSystem::insert_transition_animation(Core::Entity* p_entity, DrawSpace::Aspect::AnimationsAspect* p_anims_aspect)
{
	auto& animations_pool = p_anims_aspect->GetComponent<AnimationsAspect::AnimationsPool>("animations_pool")->getPurpose();
	auto& current_animation_name = p_anims_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose();
	if ("" == current_animation_name && animations_pool.size() > 0)
	{
		const auto& animation = animations_pool.front();

		dsstring next_animation_name = animation.first;
		for (auto& e : m_evt_handlers)
		{
			(*e)(p_entity, ANIMATION_BEGIN, next_animation_name);
		}

		// here we insert transition animation		
		dsstring last_animation_name = p_anims_aspect->GetComponent<dsstring>("last_animation_name")->getPurpose();
		if (last_animation_name != "" && last_animation_name != transitionAnimationDurationId && last_animation_name != next_animation_name)
		{
			auto& animations_table = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

			// recup anim precedente
			AnimationsAspect::AnimationRoot prev_anim = animations_table.at(last_animation_name);

			// recup prochaine anim a executer
			AnimationsAspect::AnimationRoot next_anim = animations_table.at(next_animation_name);

			if (prev_anim.ticksPerSeconds != next_anim.ticksPerSeconds)
			{
				_DSEXCEPTION("cannot build transition animation because of incompatible ticksPerSecs")
			}

			AnimationsAspect::AnimationRoot transition_anim;

			transition_anim.ticksPerSeconds = next_anim.ticksPerSeconds;
			transition_anim.duration = transitionAnimationDurationSeconds * transition_anim.ticksPerSeconds;
			transition_anim.transition_animation = true;

			for (auto& e : prev_anim.channels)
			{
				if (next_anim.channels.count(e.second.node_name))
				{
					AnimationsAspect::NodeAnimation next_anim_node = next_anim.channels.at(e.second.node_name);


					// common node between prev_anim and next_anim

					AnimationsAspect::NodeAnimation node_anim;

					node_anim.node_name = e.second.node_name;

					node_anim.position_keys.push_back(e.second.position_keys[e.second.position_keys.size() - 1]);
					node_anim.position_keys.push_back(next_anim_node.position_keys[0]);

					node_anim.position_keys[0].time_tick = 0;
					node_anim.position_keys[1].time_tick = transition_anim.duration;

					node_anim.rotations_keys.push_back(e.second.rotations_keys[e.second.rotations_keys.size() - 1]);
					node_anim.rotations_keys.push_back(next_anim_node.rotations_keys[0]);

					node_anim.rotations_keys[0].time_tick = 0;
					node_anim.rotations_keys[1].time_tick = transition_anim.duration;


					node_anim.scaling_keys.push_back(e.second.scaling_keys[e.second.scaling_keys.size() - 1]);
					node_anim.scaling_keys.push_back(next_anim_node.scaling_keys[0]);

					node_anim.scaling_keys[0].time_tick = 0;
					node_anim.scaling_keys[1].time_tick = transition_anim.duration;


					transition_anim.channels[e.second.node_name] = node_anim;
				}
			}

			std::pair<dsstring, DrawSpace::Aspect::AnimationsAspect::AnimationRoot> animation_transition_entry(transitionAnimationDurationId, transition_anim);
			animations_pool.push_front(animation_transition_entry);
		}
		///////////////////////////////////
	}

}

void AnimationsSystem::send_bones_to_shaders(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect, DrawSpace::Aspect::RenderingAspect* p_rendering_aspect)
{
	auto& bones = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose();
	auto bones_mapping = p_anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose();
	auto bones_output = p_anims_aspect->GetComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose();
	dsstring root_bone_id = p_anims_aspect->GetComponent<dsstring>("nodes_root_id")->getPurpose();

	Utils::Matrix mid;
	mid.Identity();
	if (root_bone_id != "")
	{
		read_bones_hierarchy(bones, bones_output, bones_mapping, bones.at(root_bone_id), mid);
	}
	// inject previously computed bones matrixes into each renderingnodes (one for" each passes)


	ComponentList<MesheRenderingAspectImpl::PassSlot> passes;
	p_rendering_aspect->GetComponentsByType<MesheRenderingAspectImpl::PassSlot>(passes);

	for (auto e : passes)
	{
		RenderingNode* rnode = e->getPurpose().GetRenderingNode();

		// decomposer les matrices en triplet de 3 vectors et stocker

		std::vector<Utils::Vector> bones_0;
		std::vector<Utils::Vector> bones_1;

		int vec_count = 0;

		for (size_t i = 0; i < bones_output.size(); i++)
		{
			for (size_t col = 0; col < 3; col++)
			{
				if (vec_count >= bonesBuffer0Length + bonesBuffer1Length)
				{
					_DSEXCEPTION("Too many bones");
				}
				Utils::Vector columns;
				columns[0] = bones_output[i].final_transformation(0, col);
				columns[1] = bones_output[i].final_transformation(1, col);
				columns[2] = bones_output[i].final_transformation(2, col);
				columns[3] = bones_output[i].final_transformation(3, col);

				if (vec_count <= bonesBuffer0Length - 1)
				{
					bones_0.push_back(columns);
				}
				else
				{
					bones_1.push_back(columns);
				}

				vec_count++;
			}
		}
		rnode->SetShaderArrayParameter(bonesBuffer0Id, bones_0);
		rnode->SetShaderArrayParameter(bonesBuffer1Id, bones_1);
	}
}

void AnimationsSystem::apply_animation_last_key(AnimationsAspect* p_anims_aspect)
{
	auto& bones = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose();

	dsstring& apply_animation_last_key = p_anims_aspect->GetComponent<dsstring>("apply_animation_last_key")->getPurpose();
	dsstring& last_animation_name = p_anims_aspect->GetComponent<dsstring>("last_animation_name")->getPurpose();

	if (apply_animation_last_key != "" && apply_animation_last_key != last_animation_name)
	{
		//apply last key of specified animation

		auto& animations_table = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

		if (0 == animations_table.count(apply_animation_last_key))
		{
			_DSEXCEPTION("Unknown animation name for apply_animation_last_key function");
		}

		for (auto& e : animations_table.at(apply_animation_last_key).channels)
		{
			AnimationsAspect::NodeAnimation node_animation = e.second;

			//////////////////// translations

			Utils::Matrix translation;
			translation.Identity();

			if (node_animation.position_keys.size() > 0)
			{
				translation.Translation(node_animation.position_keys[node_animation.position_keys.size() - 1].value);
			}

			//////////////////// rotations

			Utils::Matrix rotation;
			rotation.Identity();

			if (node_animation.rotations_keys.size() > 0)
			{
				node_animation.rotations_keys[node_animation.rotations_keys.size() - 1].value.RotationMatFrom(rotation);
			}

			//////////////////// scaling interpolation

			Utils::Matrix scaling;
			scaling.Identity();

			if (node_animation.scaling_keys.size() > 0)
			{
				scaling.Scale(node_animation.scaling_keys[node_animation.scaling_keys.size() - 1].value);
			}

			Utils::Matrix final_mat;
			final_mat = scaling * rotation * translation;

			if (bones.count(node_animation.node_name))
			{
				bones.at(node_animation.node_name).locale_transform = final_mat;
			}
			else
			{
				_DSEXCEPTION("invalid node name");
			}
		}

		// update last_animation_name component
		last_animation_name = apply_animation_last_key;

		// reset command component
		apply_animation_last_key = "";
	}
}

void AnimationsSystem::apply_forced_bones_pos(DrawSpace::Aspect::AnimationsAspect* p_anims_aspect)
{
	auto& bones = p_anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose();
	auto forced_bones_pos = p_anims_aspect->GetComponent<std::map<dsstring, Matrix>>("forced_bones_transformations")->getPurpose();

	for (auto& e : forced_bones_pos)
	{
		std::string bone_id = e.first;
		Matrix bone_locale_transform = e.second;

		if (bones.count(bone_id))
		{
			bones.at(bone_id).locale_transform = bone_locale_transform;
		}
	}
}

void AnimationsSystem::RegisterAnimationEvtHandler(AnimationEventHandler* p_handler)
{
	m_evt_handlers.insert(p_handler);
}

void AnimationsSystem::UnregisterAnimationEvtHandler(AnimationEventHandler* p_handler)
{
	m_evt_handlers.erase(p_handler);
}
