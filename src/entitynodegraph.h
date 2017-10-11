/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2017
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

#ifndef _ENTITYNODEGRAPH_H_
#define _ENTITYNODEGRAPH_H_

#include "entitynode.h"

namespace DrawSpace
{
namespace Systems
{
class TransformSystem;
class RenderingSystem;
class PhysicsSystem;
}
namespace EntityGraph
{
class EntityNodeGraph sealed
{
public:
	using EntityTree = st_tree::tree<Core::Entity*>;
 
private:
	mutable EntityTree                          m_tree;
    std::vector<EntityNode::EventsHandler*>     m_nodesevt_handlers;

public:
	EntityNodeGraph(void);
	~EntityNodeGraph(void);

	EntityNode SetRoot( Core::Entity* p_entity );
	void Erase(void);

    void AcceptRenderingSystem( Systems::RenderingSystem* p_renderingsystem );
    void AcceptWorldSystem( Systems::TransformSystem* p_transformsystem );
    void AcceptPhysicsSystem( Systems::PhysicsSystem* p_physicssystem );

    void RegisterNodesEvtHandler( EntityNode::EventsHandler* p_handler );
};
}
}

#endif