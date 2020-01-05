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

#include "entitynode.h"
#include "entitynodegraph.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::EntityGraph;


EntityNode::EntityNode( void ) :
    m_tree_node( NULL ),
    m_owner_graph( NULL )
{
}

EntityNode::~EntityNode( void )
{
}


EntityNode::EntityNode( EntityNode::EntityTree::node_type* p_node, /*std::vector<EntityNode::EventsHandler*>* p_nodesevt_handlers*/ EntityNodeGraph* p_owner ) :
	m_tree_node(p_node),
    m_owner_graph( p_owner )
{
    // si on passe dans ce ctor c'est que l'entitee a �t� ajout�e au graph
    // donc notif de l'evt
    std::set<EntityNode::EventsHandler*> nodesevt_handlers = m_owner_graph->m_nodesevt_handlers;
    for( auto it = nodesevt_handlers.begin(); it != nodesevt_handlers.end(); ++it )
    {
        (**it )( EntityNode::ADDED_IN_TREE, m_tree_node->data() );
    }

    // inscription dans la table EntityNodeGraph::m_entity_to_node
    m_owner_graph->m_entity_to_node[m_tree_node->data()] = m_tree_node;
}

EntityNode EntityNode::AddChild(Entity* p_entity)
{
    if( NULL == m_owner_graph )
    {
        //si m_owner_graph est a NULL c'est que cet EntityNode n'est pas ratach� a un EntityTree
        _DSEXCEPTION( "Detached node; cannot add child to it!" );
    }

    p_entity->OnAddedInGraph( m_owner_graph );

	EntityTree::node_type::iterator it = m_tree_node->insert( p_entity );
    EntityNode node(&(*it), m_owner_graph );
	return node;
}

void EntityNode::Erase(void)
{
    Entity* entity = m_tree_node->data();
    entity->OnRemovedFromGraph(m_owner_graph);

    // desinscription dans la table EntityNodeGraph::m_entity_to_node
    m_owner_graph->m_entity_to_node.erase( m_tree_node->data() );
	m_tree_node->erase();

    std::set<EntityNode::EventsHandler*> nodesevt_handlers = m_owner_graph->m_nodesevt_handlers;
    for( auto it = nodesevt_handlers.begin(); it != nodesevt_handlers.end(); ++it )
    {
        (**it )( EntityNode::REMOVED_FROM_TREE, entity );
    }
}

Core::Entity* EntityNode::GetEntity( void ) const
{
    return m_tree_node->data();
}
