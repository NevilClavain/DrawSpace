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

#include "entitynodegraph.h"
#include "renderingsystem.h"
#include "worldsystem.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Systems;

EntityNodeGraph::EntityNodeGraph(void)
{
}

EntityNodeGraph::~EntityNodeGraph(void)
{
}

EntityNode EntityNodeGraph::SetRoot(Entity* p_entity)
{
	m_tree.insert(p_entity);
	EntityNode node( &m_tree.root() );
	return node;
}

void EntityNodeGraph::Erase(void)
{
	m_tree.root().erase();
}

void EntityNodeGraph::AcceptRenderingSystem( RenderingSystem* p_renderingsystem )
{
    for( EntityTree::df_post_iterator it = m_tree.df_post_begin(); it != m_tree.df_post_end(); ++it )    
    {
        p_renderingsystem->VisitEntity( it->data() );
    }
}

void EntityNodeGraph::AcceptWorldSystem( WorldSystem* p_worldsystem )
{
    for( EntityTree::df_pre_iterator it = m_tree.df_pre_begin(); it != m_tree.df_pre_end(); ++it )
    {
        if( it->is_root() )
        {
            p_worldsystem->VisitEntity( NULL, it->data() );
        }
        else
        {
            p_worldsystem->VisitEntity( it->parent().data(), it->data() );
        }
    }
}