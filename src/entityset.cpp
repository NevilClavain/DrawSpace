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
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Interface;


EntitySet::EntitySet( void )
{
}

EntitySet::~EntitySet( void )
{

}

void EntitySet::AcceptSystemTopDownRecursive( System* p_system )
{
    for( auto it = m_entities_containers.begin(); it != m_entities_containers.end(); ++it )
    {
        EntityTreeContainer* current_tree = *it;
        current_tree->AcceptSystemTopDownRecursive( p_system );
    }
}

void EntitySet::AcceptSystemLeafsToTopRecursive( System* p_system )
{
 
    for( auto it = m_entities_containers.begin(); it != m_entities_containers.end(); ++it )
    {
        EntityTreeContainer* current_tree = *it;
        current_tree->AcceptSystemLeafsToTopRecursive( p_system );
    }
}

void EntitySet::AddRoot( Entity* p_elt )
{
    EntityTreeContainer* etc = _DRAWSPACE_NEW_( EntityTreeContainer, EntityTreeContainer );
    etc->AddRoot( p_elt );
    m_entities_containers.push_back( etc );
}
