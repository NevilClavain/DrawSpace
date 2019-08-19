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
#include "mesherenderingaspectimpl.h"

using namespace DrawSpace;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::Core;
using namespace DrawSpace::AspectImplementations;

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

void AnimationsSystem::read_bones_hierarchy(const std::map<dsstring, AnimationsAspect::Bone>& p_bones, AnimationsAspect::Bone p_bone_node, const DrawSpace::Utils::Matrix& p_parent_transform)
{
	Matrix locale_node_transform = p_bone_node.locale_transform;
	Matrix global_transformation = locale_node_transform * p_parent_transform;

	for (auto& e : p_bone_node.children)
	{
		std::string id = e;
		AnimationsAspect::Bone child = p_bones.at(id);
		read_bones_hierarchy(p_bones, child, global_transformation);
	}
}

void AnimationsSystem::VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity)
{
    AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
    if (anims_aspect)
    {

		dsstring root_bone_id = anims_aspect->GetComponent<dsstring>("bones_root")->getPurpose();

		auto bones = anims_aspect->GetComponent<std::map<dsstring, AnimationsAspect::Bone>>("bones")->getPurpose();

		Utils::Matrix mid;
		mid.Identity();
		read_bones_hierarchy(bones, bones.at(root_bone_id), mid);


		// inject previously computed bones matrixes into each renderingnodes (one for each passes)
		/*

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

			std::vector<Utils::Vector> bones_0;
			bones_0.resize(69);

			Utils::Vector test_color(0.0, 0.6999, 0.0, 1.0);
			bones_0[68] = test_color;

			rnode->SetShaderArrayParameter("bones_0", bones_0);
		}
		*/
    }
}