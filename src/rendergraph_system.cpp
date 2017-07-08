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
#include "colorarg_component.h"
#include "text_component.h"

#include "plugin.h"
#include "renderer.h"

#include "memalloc.h"

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
    //if( CHECK_DS_COMPONENT( p_entity, DrawSpace::RenderingQueueComponentType ) )

    if( p_entity->CheckComponent( RenderingQueueComponentType ) )
    {
        //RenderingQueueComponent* renderingqueue_comp = EXTRACT_DS_COMPONENT( p_entity, RenderingQueueComponent, RenderingQueueComponentType, 0 )

        RenderingQueueComponent* renderingqueue_comp = p_entity->ExtractComponent<RenderingQueueComponent>( RenderingQueueComponentType, 0 );

        renderingqueue_comp->m_queue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue );

        //if( CHECK_DS_COMPONENT( p_entity, DrawSpace::ColorArgComponentType ) )

        if( p_entity->CheckComponent( ColorArgComponentType ) )
        {
            //ColorArgComponent* color_comp = EXTRACT_DS_COMPONENT( p_entity, ColorArgComponent, ColorArgComponentType, 0 )
            ColorArgComponent* color_comp = p_entity->ExtractComponent<ColorArgComponent>( ColorArgComponentType, 0 );

            renderingqueue_comp->m_queue->SetTargetClearingColor( color_comp->m_r, color_comp->m_g, color_comp->m_b, color_comp->m_a );    

            renderingqueue_comp->m_queue->EnableTargetClearing( true );
        }
    }
}

void RenderGraphSystem::phase_release( Entity* p_entity )
{
    //if( CHECK_DS_COMPONENT( p_entity, DrawSpace::RenderingQueueComponentType ) )
    if( p_entity->CheckComponent(RenderingQueueComponentType ) )
    {        
        //RenderingQueueComponent* renderingqueue_comp = EXTRACT_DS_COMPONENT( p_entity, RenderingQueueComponent, RenderingQueueComponentType, 0 )
        
        RenderingQueueComponent* renderingqueue_comp = p_entity->ExtractComponent<RenderingQueueComponent>( RenderingQueueComponentType, 0 );

        _DRAWSPACE_DELETE_( renderingqueue_comp->m_queue );
    }
}

void RenderGraphSystem::phase_run( Entity* p_entity )
{
    //if( CHECK_DS_COMPONENT( p_entity, DrawSpace::RenderingQueueComponentType ) )
    if( p_entity->CheckComponent( RenderingQueueComponentType ) )
    {
        //RenderingQueueComponent* renderingqueue_comp = EXTRACT_DS_COMPONENT( p_entity, RenderingQueueComponent, RenderingQueueComponentType, 0 )

        RenderingQueueComponent* renderingqueue_comp = p_entity->ExtractComponent<RenderingQueueComponent>( RenderingQueueComponentType, 0 );

        renderingqueue_comp->m_queue->Draw();
    }

    //if( CHECK_DS_COMPONENT( p_entity, DrawSpace::TextComponentType ) )
    if( p_entity->CheckComponent( TextComponentType ) )
    {         
        //TextComponent* text_comp = EXTRACT_DS_COMPONENT( p_entity, TextComponent, TextComponentType, 0 )

        TextComponent* text_comp = p_entity->ExtractComponent<TextComponent>( TextComponentType, 0 );
        
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;        
        renderer->DrawText( text_comp->r, text_comp->g, text_comp->b, text_comp->x, text_comp->y, text_comp->text.c_str() );
    }
}
