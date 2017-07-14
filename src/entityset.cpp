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

#include "entityset.h"
#include "system.h"
using namespace DrawSpace;
using namespace DrawSpace::Interface;


void EntitySet::InsertTree( const st_tree::tree<Entity*>& p_tree )
{
    m_entities.push_back( p_tree );
}

void EntitySet::AcceptSystemTopDownRecursive( System* p_system, Phase p_phase )
{
    for( auto it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        st_tree::tree<Entity*> current_tree = *it;
                       
        for( st_tree::tree<Entity*>::df_pre_iterator it2(current_tree.df_pre_begin() ); it2 != current_tree.df_pre_end(); ++it2 )
        {
            Entity* curr_entity = it2->data();

            p_system->VisitEntitySet( curr_entity, p_phase );
        }  
    }
}

void EntitySet::AcceptSystemLeafsToTopRecursive( System* p_system, Phase p_phase )
{
    for( auto it = m_entities.begin(); it != m_entities.end(); ++it )
    {
        st_tree::tree<Entity*> current_tree = *it;
                       
        for( st_tree::tree<Entity*>::df_post_iterator it2(current_tree.df_post_begin() ); it2 != current_tree.df_post_end(); ++it2 )
        {
            Entity* curr_entity = it2->data();

            p_system->VisitEntitySet( curr_entity, p_phase );
        }  
    }
}