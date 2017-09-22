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
    m_tree_node( NULL )
{
}

EntityNode::EntityNode( EntityNode::EntityTree::node_type* p_node ) :
	m_tree_node(p_node)
{
}

EntityNode EntityNode::AddChild(Entity* p_entity)
{
	EntityTree::node_type::iterator it = m_tree_node->insert( p_entity );
	EntityNode node(&(*it));
	return node;
}

void EntityNode::Erase(void)
{
	m_tree_node->erase();
}