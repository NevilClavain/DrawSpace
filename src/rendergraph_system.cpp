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

#include "rendergraph_system.h"

#include "components_ids.h"
#include "renderingqueue_component.h"

using namespace DrawSpace;

void RenderGraphSystem::VisitEntitySet( Entity* p_entity, EntitySet::Phase p_phase )
{
    switch( p_phase )
    {
        case EntitySet::PHASE_INIT:
            
            phase_init( p_entity );
            break;

        case EntitySet::PHASE_RELEASE:

            phase_release( p_entity );
            break;

        case EntitySet::PHASE_RUN:

            phase_run( p_entity );
            break;
    }
}

void RenderGraphSystem::phase_init( Entity* p_entity )
{
    if( p_entity->count( DrawSpace::RenderingQueueComponentType ) )
    {
        _asm nop
    }

    if( p_entity->count( DrawSpace::ColorArgComponentType ) )
    {
        _asm nop    
    }
}

void RenderGraphSystem::phase_release( Entity* p_entity )
{
}

void RenderGraphSystem::phase_run( Entity* p_entity )
{
}
