#pragma once

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

#include "systems.h"
#include "entitynodegraph.h"
#include "animationsaspect.h"

namespace DrawSpace
{
namespace Systems
{
class AnimationsSystem : public Interface::System
{
protected:

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

	//recursive
	void read_bones_hierarchy(const std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Bone>& p_bones,
		std::vector<DrawSpace::Aspect::AnimationsAspect::BoneOutput>& p_bones_output,
		const std::map<dsstring, int>& p_bones_mapping, DrawSpace::Aspect::AnimationsAspect::Bone p_bone_node,
		const DrawSpace::Utils::Matrix& p_parent_transform);

public:
    AnimationsSystem();
    ~AnimationsSystem();

    dsstring GetSystemId(void) const { return "AnimationsSystem"; };
    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);
};
}
}