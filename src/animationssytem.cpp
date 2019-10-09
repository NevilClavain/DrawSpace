/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2019
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
#include "renderingaspect.h"
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

	if (p_node.position_keys.size() > 0)
	{
		Utils::Vector v_interpolated;
		if (p_node.position_keys.size() < 2)
		{
			v_interpolated = p_node.position_keys[0].value;
		}
		else
		{
			for (size_t i = 0 ; i < p_node.position_keys.size() - 1; i++)
			{
				if (p_node.position_keys[i].time_tick <= p_current_tick && p_current_tick < p_node.position_keys[i + 1].time_tick)
				{
					AnimationsAspect::VectorKey kA = p_node.position_keys[i];
					AnimationsAspect::VectorKey kB = p_node.position_keys[i + 1];

					dsreal blend = (p_current_tick - kA.time_tick) / (kB.time_tick - kA.time_tick);
					v_interpolated = Utils::Vector::Lerp(kA.value, kB.value, blend);
					break;
				}
			}
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
					break;
				}
			}
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
					break;
				}
			}
		}
		scaling.Scale(v_interpolated);
	}

	///////////////////////

	// final = translation * rotation * scaling

	p_out_matrix = scaling * rotation * translation;
}

void AnimationsSystem::VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity)
{
    AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
    if (anims_aspect)
    {		
		dsstring root_bone_id = anims_aspect->GetComponent<dsstring>("nodes_root_id")->getPurpose();

		auto bones = anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Node>>("nodes")->getPurpose();

		////////////////////////////////////////////////////////////////////////////////////////////////
		//// if active animation, process it to compute bones matrix result

		dsstring current_anim_name = anims_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose();

		if ("" != current_anim_name)
		{
			auto& animations_data = anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::AnimationRoot>>("animations")->getPurpose();

			if (0 == animations_data.count(current_anim_name))
			{
				_DSEXCEPTION("Unknown animation name");
			}

			AnimationsAspect::AnimationRoot current_animation_data = animations_data.at(current_anim_name);

			TimeAspect::TimeMark tmk = anims_aspect->GetComponent<TimeAspect::TimeMark>("current_animation_timemark")->getPurpose();

			long tms = tmk.GetTimeMs();
			dsreal nb_seconds = (dsreal)tms / 1000.0;
			dsreal nb_ticks = anims_aspect->GetComponent<long>("current_animation_ticks_per_seconds")->getPurpose() * nb_seconds;

			if (nb_ticks < anims_aspect->GetComponent<dsreal>("current_animation_ticks_duration")->getPurpose())
			{
				anims_aspect->GetComponent<dsreal>("current_animation_seconds_progress")->getPurpose() = nb_seconds;
				anims_aspect->GetComponent<dsreal>("current_animation_ticks_progress")->getPurpose() = nb_ticks;
				
				for (auto& e : current_animation_data.channels)
				{
					Utils::Matrix bone_locale_transform;
					compute_node_animationresult_matrix(e.second, nb_ticks, bone_locale_transform);

					if (bones.count(e.second.node_name))
					{
						bones.at(e.second.node_name).locale_transform = bone_locale_transform;
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
				anims_aspect->GetComponent<dsstring>("current_animation_name")->getPurpose() = "";
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		//// get some eventually forced bones position

		auto forced_bones_pos = anims_aspect->GetComponent<std::map<dsstring, Matrix>>("forced_bones_transformations")->getPurpose();

		for (auto& e : forced_bones_pos)
		{
			std::string bone_id = e.first;
			Matrix bone_locale_transform = e.second;

			if (bones.count(bone_id))
			{
				bones.at(bone_id).locale_transform = bone_locale_transform;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		///// Push bones matrix to shader

		auto bones_mapping = anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose();
		auto bones_output = anims_aspect->GetComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose();

		Utils::Matrix mid;
		mid.Identity();
		if (root_bone_id != "")
		{
			read_bones_hierarchy(bones, bones_output, bones_mapping, bones.at(root_bone_id), mid);
		}
		// inject previously computed bones matrixes into each renderingnodes (one for" each passes)
		RenderingAspect* rendering_aspect = p_entity->GetAspect <RenderingAspect>();
		if (!rendering_aspect)
		{
			_DSEXCEPTION("An entity with AnimationsAspect must also have a RenderingAspect");
		}

		ComponentList<MesheRenderingAspectImpl::PassSlot> passes;
		rendering_aspect->GetComponentsByType<MesheRenderingAspectImpl::PassSlot>(passes);

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

					if (vec_count <= bonesBuffer0Length-1)
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
}