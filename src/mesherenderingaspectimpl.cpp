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

#include "mesherenderingaspectimpl.h"
#include "renderingaspect.h"
#include "worldaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

MesheRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
    m_pass_name( p_pass_name )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    m_rendering_node = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );

    m_cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) );
    m_rendering_node->RegisterHandler( m_cb );

    //m_world.Identity();
    m_world.Translation( Vector( 0.0, 0.0, -3.0, 1.0 ) );

    m_view.Identity();
    m_proj.Perspective( 1.0, 0.75, 1.0, 100000000000.0 );
}

MesheRenderingAspectImpl::PassSlot::~PassSlot( void )
{
    _DRAWSPACE_DELETE_( m_rendering_node );
}       

void MesheRenderingAspectImpl::PassSlot::on_renderingnode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    m_renderer->DrawMeshe( m_world, m_view, m_proj );
}

MesheRenderingAspectImpl::MesheRenderingAspectImpl( void )
{
}

bool MesheRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, RenderingQueue* p_passqueue )
{
    bool updated_queue = false;
    std::vector<Component<PassSlot>*> pass_slots;
    m_owner->GetComponentsByType<PassSlot>( pass_slots );

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

void MesheRenderingAspectImpl::RegisterToRendering( RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void MesheRenderingAspectImpl::UnregisterFromRendering( RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void MesheRenderingAspectImpl::run( Entity* p_entity )
{
    WorldAspect* world_aspect = p_entity->GetAspect<WorldAspect>();

    if( world_aspect )
    {
        Matrix world;
        world_aspect->GetWorldTransform( world );

        // redistribution de la transfo world...

        std::vector<Component<PassSlot>*> pass_slots;
        m_owner->GetComponentsByType<PassSlot>( pass_slots );

        for( size_t i = 0; i < pass_slots.size(); i++ )
        {
            pass_slots[i]->getPurpose().m_world = world;
        }
    }
}