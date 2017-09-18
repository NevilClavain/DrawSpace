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

#include "mesherenderingaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

void MesheRenderingAspect::PassSlot::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix world;
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    view.Identity();

    world.Translation( Vector( 0.0, 0.0, -3.0, 1.0 ) );

    proj.Perspective( 1.0, 0.75, 1.0, 100000000000.0 );

    m_renderer->DrawMeshe( world, view, proj );
}

MesheRenderingAspect::MesheRenderingAspect( void )
{
}

bool MesheRenderingAspect::VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue )
{
    bool updated_queue = false;
    std::vector<Component<PassSlot>*> pass_slots;
    GetComponentsByType<PassSlot>( pass_slots );

    for( size_t i = 0; i < pass_slots.size(); i++ )
    {
        if( pass_slots[i]->getPurpose().m_pass_name == p_name )
        {
            if( m_add_in_rendergraph )
            {
                // ajout du renderingnode dans la renderingqueue  

                p_passqueue->Add( pass_slots[i]->getPurpose().m_rendering_node );
            }
            else
            {
                // suppression du renderingnode de la renderingqueue

                p_passqueue->Remove( pass_slots[i]->getPurpose().m_rendering_node );
            }
            updated_queue = true;
        }
    }
    return updated_queue;
}

void MesheRenderingAspect::RegisterToRendering( const RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void MesheRenderingAspect::UnregisterFromRendering( const RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}
