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

#include "entitynode.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;


EntityNode::EntityNode( void ) :
    m_tree_node( NULL ),
    m_nodesevt_handlers( NULL )
{
}


EntityNode::EntityNode( EntityNode::EntityTree::node_type* p_node, std::vector<EntityNode::EventsHandler*>* p_nodesevt_handlers ) :
	m_tree_node(p_node),
    m_nodesevt_handlers( p_nodesevt_handlers )
{
    // si on passe dans ce ctor c'est que l'entitee a été ajoutée au graph
    // donc notif de l'evt
    for( size_t i = 0; i < m_nodesevt_handlers->size(); i++ )
    {
        EntityNode::EventsHandler* curr_h = (*m_nodesevt_handlers)[i];
        ( *curr_h )( EntityNode::ADDED_IN_TREE, m_tree_node->data() );
    }
}

EntityNode EntityNode::AddChild(Entity* p_entity)
{
    if( NULL == m_nodesevt_handlers )
    {
        //si m_nodesevt_handlers est a NULL c'est que cet EntityNode n'est pas rataché a un EntityTree
        _DSEXCEPTION( "Detached node; cannot add child to it!" );
    }

	EntityTree::node_type::iterator it = m_tree_node->insert( p_entity );
	EntityNode node(&(*it), m_nodesevt_handlers );
	return node;
}

void EntityNode::Erase(void)
{
    Entity* entity = m_tree_node->data();
	m_tree_node->erase();

    for( size_t i = 0; i < m_nodesevt_handlers->size(); i++ )
    {
        EntityNode::EventsHandler* curr_h = (*m_nodesevt_handlers)[i];
        ( *curr_h )( EntityNode::REMOVED_FROM_TREE, entity );
    }
}