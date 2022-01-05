/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2022
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

#ifndef _ENTITYNODE_H_
#define _ENTITYNODE_H_

#include "st_tree.h"
#include "entity.h"
#include "callback.h"

namespace DrawSpace
{
namespace EntityGraph
{
class EntityNodeGraph;
class EntityNode
{
public:

    typedef enum
    {
        ADDED_IN_TREE,
		REMOVED_FROM_TREE,

    } Event;

    using EventsHandler = DrawSpace::Core::BaseCallback2<void, Event, Core::Entity*>;
   
    // no need of copy ctor, because m_tree_node and m_owner_graph are shared by all instances
    // (they represent not private resources as it's the case in RAII for example)
private:
	using EntityTree = st_tree::tree<Core::Entity*>;

	EntityTree::node_type*                      m_tree_node;
    EntityNodeGraph*                            m_owner_graph;

    // personne n'a le droit d'appeler ce ctor directement hormis EntityNodeGraph (friend)
    EntityNode( EntityTree::node_type* p_node, EntityNodeGraph* p_owner );

public:
    EntityNode( void );
    ~EntityNode( void );

    Core::Entity* GetEntity( void ) const;

	EntityNode AddChild( Core::Entity* p_entity );
	void Erase( void );

    friend class EntityNodeGraph;
};
}
}

#endif
