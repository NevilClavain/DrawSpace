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
#include "viewportquad_component.h"
#include "rendertarget_component.h"

#include "plugin.h"
#include "renderer.h"

#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;

void RenderGraphSystem::VisitEntitySet( Entity* p_entity/*, EntitySet::Phase p_phase*/ )
{
    if( p_entity->CheckComponent( RenderingQueueComponentType ) )
    {
        RenderingQueueComponent* renderingqueue_comp = p_entity->ExtractComponent<RenderingQueueComponent>( RenderingQueueComponentType, 0 );

        renderingqueue_comp->m_queue->Draw();
    }

    if( p_entity->CheckComponent( TextComponentType ) )
    {         
        TextComponent* text_comp = p_entity->ExtractComponent<TextComponent>( TextComponentType, 0 );
        
        DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;        
        renderer->DrawText( text_comp->m_r, text_comp->m_g, text_comp->m_b, text_comp->m_x, text_comp->m_y, text_comp->m_text.c_str() );
    }
}

