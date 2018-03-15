/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2018                        
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
/* -*-LIC_END-*- */

#include "quadrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;

QuadRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
m_pass_name( p_pass_name ),
m_renderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface ),
m_rendering_node( _DRAWSPACE_NEW_( RenderingNode, RenderingNode ) ),
m_cb( _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) ) )
{
    m_rendering_node->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    m_rendering_node->RegisterHandler( m_cb );

    m_world.Identity();


    // construire le meshe

    Vertex v1, v2, v3, v4;
    v1.x = - 1.0 / 2.0;
    v1.y = 1.0 / 2.0;
    v1.z = 0.0;
    v1.tu[0] = 0.0;
    v1.tv[0] = 0.0;

    v2.x = 1.0 / 2.0;
    v2.y = 1.0 / 2.0;
    v2.z = 0.0;
    v2.tu[0] = 1.0;
    v2.tv[0] = 0.0;

    v3.x = 1.0 / 2.0;
    v3.y = - 1.0 / 2.0;
    v3.z = 0.0;
    v3.tu[0] = 1.0;
    v3.tv[0] = 1.0;

    v4.x = - 1.0 / 2.0;
    v4.y = - 1.0 / 2.0;
    v4.z = 0.0;
    v4.tu[0] = 0.0;
    v4.tv[0] = 1.0;

    m_rendering_node->GetMeshe()->AddVertex( v1 );
    m_rendering_node->GetMeshe()->AddVertex( v2 );
    m_rendering_node->GetMeshe()->AddVertex( v3 );
    m_rendering_node->GetMeshe()->AddVertex( v4 );

    m_rendering_node->GetMeshe()->AddTriangle( Triangle( 0, 2, 1 ) );
    m_rendering_node->GetMeshe()->AddTriangle( Triangle( 0, 3, 2 ) );

    // construire un matrice projection classique
    DrawSpace::Interface::Renderer::Characteristics characteristics;
    DrawSpace::Interface::Renderer* renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
    renderer->GetRenderCharacteristics( characteristics );
    m_proj.Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 10.0 );

}

QuadRenderingAspectImpl::PassSlot::~PassSlot( void )
{
    Meshe* meshe = m_rendering_node->GetMeshe();
    _DRAWSPACE_DELETE_( meshe );
    _DRAWSPACE_DELETE_( m_rendering_node );
    _DRAWSPACE_DELETE_( m_cb );
}       

void QuadRenderingAspectImpl::PassSlot::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    Matrix view;
    view.Identity();
    m_renderer->DrawMeshe( m_world, view, m_proj );
}


QuadRenderingAspectImpl::QuadRenderingAspectImpl( void )
{
}

bool QuadRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    ComponentList<PassSlot> pass_slots;

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

void QuadRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void QuadRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void QuadRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

    if( transform_aspect )
    {
        Matrix world;
        transform_aspect->GetWorldTransform( world );

        // redistribution de la transfo world...

        ComponentList<PassSlot> pass_slots;
        m_owner->GetComponentsByType<PassSlot>( pass_slots );

        for( size_t i = 0; i < pass_slots.size(); i++ )
        {
            pass_slots[i]->getPurpose().m_world = world;
        }
    }
}