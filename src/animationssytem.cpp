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
#include "animationsaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Systems;
using namespace DrawSpace::Aspect;

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

void AnimationsSystem::VisitEntity(Core::Entity* p_parent, Core::Entity* p_entity)
{
    AnimationsAspect* anims_aspect = p_entity->GetAspect<AnimationsAspect>();
    if (anims_aspect)
    {

    }
}