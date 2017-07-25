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

#include "tree_container.h"
#include "system.h"

using namespace DrawSpace;

void EntityTreeContainer::AddRoot( Entity* p_elt )
{
    m_tr.insert( p_elt );
    m_nodes["root"] = &m_tr.root();
}

void EntityTreeContainer::AddLeaf( Entity* p_elt, const std::vector<int> p_indexes )
{
    char comment[32];

    dsstring indexes_sig = "root";
    for( size_t i = 0; i < p_indexes.size(); i++ )
    {
        sprintf( comment, ".%d", p_indexes[i] );
        indexes_sig += comment;
    }

    m_nodes[indexes_sig]->insert( p_elt );

    sprintf( comment, ".%d", m_nodes[indexes_sig]->size() - 1 );
    dsstring new_indexes_sig = indexes_sig + comment;

    m_nodes[new_indexes_sig] = &(*m_nodes[indexes_sig])[m_nodes[indexes_sig]->size() - 1];
}

void EntityTreeContainer::AcceptSystemTopDownRecursive( Interface::System* p_system )
{
    for( st_tree::tree<Entity*>::df_pre_iterator it2( m_tr.df_pre_begin() ); it2 != m_tr.df_pre_end(); ++it2 )
    {
        Entity* curr_entity = it2->data();

        p_system->VisitEntitySet( curr_entity );
    }  
}

void EntityTreeContainer::AcceptSystemLeafsToTopRecursive( Interface::System* p_system )
{
    for( st_tree::tree<Entity*>::df_post_iterator it2( m_tr.df_post_begin() ); it2 != m_tr.df_post_end(); ++it2 )
    {
        Entity* curr_entity = it2->data();

        p_system->VisitEntitySet( curr_entity );
    }  
}