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

#include "rendergraph_data.h"

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

void RenderGraphData::initialize_new_entity( Entity* p_entity )
{
    if( p_entity->CheckComponent( RenderingQueueComponentType ) )
    {
        RenderingQueueComponent* renderingqueue_comp = p_entity->ExtractComponent<RenderingQueueComponent>( RenderingQueueComponentType, 0 );
        
        if( p_entity->CheckComponent( RenderTargetComponentType ) )
        {
            RenderTargetComponent* rendertarger_comp = p_entity->ExtractComponent<RenderTargetComponent>( RenderTargetComponentType, 0 );

            long h_resol, w_resol;

            if( rendertarger_comp->m_targetdims_fromrenderer )
            {
                DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

                DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
                renderer->GetRenderCharacteristics( renderer_characteristics );

                h_resol = renderer_characteristics.height_resol;
                w_resol = renderer_characteristics.width_resol;

            }
            else
            {
                h_resol = rendertarger_comp->m_targetdims_height;
                w_resol = rendertarger_comp->m_targetdims_width;
            }
        
            rendertarger_comp->m_targettexture = _DRAWSPACE_NEW_( Texture, Texture( rendertarger_comp->m_name, true, w_resol, h_resol, rendertarger_comp->m_renderpurpose, rendertarger_comp->m_rendertarget ) );
            
            renderingqueue_comp->m_queue = _DRAWSPACE_NEW_( RenderingQueue, RenderingQueue( rendertarger_comp->m_targettexture ) );

            if( rendertarger_comp->m_destination.first && rendertarger_comp->m_destination.second > -1 )
            {
                rendertarger_comp->m_destination.first->m_viewportquad->SetTexture( rendertarger_comp->m_targettexture, rendertarger_comp->m_destination.second );
            }
        }
        else
        {
            renderingqueue_comp->m_queue = _DRAWSPACE_NEW_( Core::RenderingQueue, Core::RenderingQueue );        
        }

        if( p_entity->CheckComponent( ColorArgComponentType ) )
        {
            ColorArgComponent* color_comp = p_entity->ExtractComponent<ColorArgComponent>( ColorArgComponentType, 0 );

            renderingqueue_comp->m_queue->SetTargetClearingColor( color_comp->m_r, color_comp->m_g, color_comp->m_b, color_comp->m_a );    

            renderingqueue_comp->m_queue->EnableTargetClearing( true );
        }

        if( p_entity->CheckComponent( ViewportQuadComponentType ) )
        {
            ViewportQuadComponent* viewportquad_comp = p_entity->ExtractComponent<ViewportQuadComponent>( ViewportQuadComponentType, 0 );

            if( viewportquad_comp->m_dims_from_renderer )
            {
                DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
                DrawSpace::Interface::Renderer::Characteristics renderer_characteristics;
                renderer->GetRenderCharacteristics( renderer_characteristics );
            
                viewportquad_comp->m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( renderer_characteristics.width_viewport, renderer_characteristics.height_viewport, viewportquad_comp->m_zoffset ) );
            }
            else
            {
                viewportquad_comp->m_viewportquad = _DRAWSPACE_NEW_( ViewportQuad, ViewportQuad( viewportquad_comp->m_width, viewportquad_comp->m_height, viewportquad_comp->m_zoffset ) );
            }

            renderingqueue_comp->m_queue->Add( viewportquad_comp->m_viewportquad );
        }
    }
}

void RenderGraphData::AddRoot( Entity* p_elt )
{
    initialize_new_entity( p_elt );
    EntitySet::AddRoot( p_elt );
}