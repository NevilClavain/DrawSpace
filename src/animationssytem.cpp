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
	m_rot_inc = true;
}

AnimationsSystem::~AnimationsSystem(void)
{
}

void AnimationsSystem::run(EntityGraph::EntityNodeGraph* p_entitygraph)
{
    p_entitygraph->AcceptSystemRootToLeaf(this);
}

void AnimationsSystem::read_bones_hierarchy(const std::map<dsstring, AnimationsAspect::Bone>& p_bones, 
											std::vector<AnimationsAspect::BoneOutput>& p_bones_output, 
											const std::map<dsstring, int>& p_bones_mapping, 
											AnimationsAspect::Bone p_bone_node, 
											const DrawSpace::Utils::Matrix& p_parent_transform)
{
	Matrix locale_node_transform = p_bone_node.locale_transform;
	Matrix global_transformation = locale_node_transform * p_parent_transform;

	Matrix final_transformation = p_bones_output.at(p_bones_mapping.at(p_bone_node.id)).offset_matrix * global_transformation;

	p_bones_output.at(p_bones_mapping.at(p_bone_node.id)).final_transformation = final_transformation;

	for (auto& e : p_bone_node.children)
	{
		std::string id = e;
		AnimationsAspect::Bone child = p_bones.at(id);
		read_bones_hierarchy(p_bones, p_bones_output, p_bones_mapping, child, global_transformation);
	}
}

void AnimationsSystem::VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity)
{
    AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
    if (anims_aspect)
    {

		dsstring root_bone_id = anims_aspect->GetComponent<dsstring>("bones_root")->getPurpose();

		auto bones = anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Bone>>("bones")->getPurpose();
		auto bones_mapping = anims_aspect->GetComponent<std::map<dsstring, int>>("bones_mapping")->getPurpose();
		auto bones_output = anims_aspect->GetComponent<std::vector<AnimationsAspect::BoneOutput>>("bones_outputs")->getPurpose();

		//////////// temporaire tests animation

		static bool once = false;

		TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

		TimeAspect* time_aspect = transform_aspect->GetTimeAspectRef();

		if (!once)
		{
			m_rotation = time_aspect->TimeAngleFactory(0.0);
			once = true;
		}
		else
		{
			Matrix rot;
			Matrix trans_arm;

			trans_arm.Translation(0.0, 1.5, 0.0);

			rot.Rotation(Vector(0.0, 0.0, 1.0, 1.0), Utils::Maths::DegToRad(m_rotation.GetValue()));


			bones["neck_pivot"].locale_transform = rot;

			bones["neck_arm"].locale_transform = trans_arm;


			if (m_rot_inc)
			{
				m_rotation.Increase(90.0);
				if (m_rotation.GetValue() > 40.0 && m_rotation.GetValue() < 180.0)
				{
					m_rot_inc = false;
				}
			}
			else
			{
				m_rotation.Decrease(90.0);
				if (m_rotation.GetValue() < 360.0 - 40.0 && m_rotation.GetValue() > 180.0)
				{
					m_rot_inc = true;
				}
			}
		}



		////////////////////////////


		Utils::Matrix mid;
		mid.Identity();
		read_bones_hierarchy(bones, bones_output, bones_mapping, bones.at(root_bone_id), mid);

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