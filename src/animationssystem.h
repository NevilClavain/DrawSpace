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
#include "timeaspect.h"

namespace DrawSpace
{
namespace Systems
{
class AnimationsSystem : public Interface::System
{
public:

	static const int		bonesBuffer0StartReg	= 31;
	static const int		bonesBuffer0Length		= 69;

	static const int		bonesBuffer1StartReg	= 124;
	static const int		bonesBuffer1Length		= 132;

	static const dsstring	bonesBuffer0Id;
	static const dsstring	bonesBuffer1Id;

protected:

    void run(EntityGraph::EntityNodeGraph* p_entitygraph);

	//recursive
	void read_bones_hierarchy(const std::map<dsstring, DrawSpace::Aspect::AnimationsAspect::Node>& p_nodes,
		std::vector<DrawSpace::Aspect::AnimationsAspect::BoneOutput>& p_bones_output,
		const std::map<dsstring, int>& p_bones_mapping, DrawSpace::Aspect::AnimationsAspect::Node p_node,
		const DrawSpace::Utils::Matrix& p_parent_transform);

	void compute_node_animationresult_matrix(const DrawSpace::Aspect::AnimationsAspect::NodeAnimation& p_node, dsreal p_current_tick, Utils::Matrix& p_out_matrix) const;

public:
    AnimationsSystem();
    ~AnimationsSystem();

    dsstring GetSystemId(void) const { return "AnimationsSystem"; };
    void VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity);
};
}
}