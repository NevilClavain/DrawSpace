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


#include "impostorsrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;


ImpostorsRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
m_pass_name( p_pass_name ),
m_renderer( DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface ),
m_rendering_node( _DRAWSPACE_NEW_( RenderingNode, RenderingNode ) ),
m_cb( _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) ) )
{
    m_rendering_node->SetMeshe( _DRAWSPACE_NEW_( Meshe, Meshe ) );
    m_rendering_node->RegisterHandler( m_cb );

    m_world.Identity();
    m_view.Identity();
    m_proj.Identity();
}

ImpostorsRenderingAspectImpl::PassSlot::~PassSlot( void )
{
    Meshe* meshe = m_rendering_node->GetMeshe();
    _DRAWSPACE_DELETE_( meshe );
    _DRAWSPACE_DELETE_( m_rendering_node );
    _DRAWSPACE_DELETE_( m_cb );
}       

void ImpostorsRenderingAspectImpl::PassSlot::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    m_renderer->DrawMeshe( m_world, m_view, m_proj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImpostorsRenderingAspectImpl::ImpostorsRenderingAspectImpl( void )
{
}

void ImpostorsRenderingAspectImpl::build_quads( const PassSlot& p_pass_slot )
{
    ComponentList<ImpostorDescriptor> impostors;
    m_owner->GetComponentsByType<ImpostorDescriptor>( impostors );
    
    Meshe* meshe = p_pass_slot.GetRenderingNode()->GetMeshe();

    for( size_t i = 0; i < impostors.size(); ++i )
    {
        ImpostorDescriptor descr = impostors[i]->getPurpose();

        Vertex v1, v2, v3, v4;

        // vertex x,y,z set by impostors shaders
        v1.x = 0.0;
        v1.y = 0.0;
        v1.z = 0.0;

        v1.tu[0] = descr.u1;
        v1.tv[0] = descr.v1;
        v1.nx = 1.0;

        v1.tu[7] = descr.localpos[0];
        v1.tv[7] = descr.localpos[1];
        v1.tw[7] = descr.localpos[2];
        v1.tu[8] = descr.width_scale;
        v1.tv[8] = descr.height_scale;

        // vertex x,y,z set by impostors shaders
        v2.x = 0.0;
        v2.y = 0.0;
        v2.z = 0.0;

        v2.tu[0] = descr.u2;
        v2.tv[0] = descr.v2;
        v2.nx = 2.0;



        v2.tu[7] = descr.localpos[0];
        v2.tv[7] = descr.localpos[1];
        v2.tw[7] = descr.localpos[2];
        v2.tu[8] = descr.width_scale;
        v2.tv[8] = descr.height_scale;

        // vertex x,y,z set by impostors shaders
        v3.x = 0.0;
        v3.y = 0.0;
        v3.z = 0.0;

        v3.tu[0] = descr.u3;
        v3.tv[0] = descr.v3;
        v3.nx = 3.0;

 

        v3.tu[7] = descr.localpos[0];
        v3.tv[7] = descr.localpos[1];
        v3.tw[7] = descr.localpos[2];
        v3.tu[8] = descr.width_scale;
        v3.tv[8] = descr.height_scale;

        // vertex x,y,z set by impostors shaders
        v4.x = 0.0;
        v4.y = 0.0;
        v4.z = 0.0;

        v4.tu[0] = descr.u4;
        v4.tv[0] = descr.v4;
        v4.nx = 4.0;

        v4.tu[7] = descr.localpos[0];
        v4.tv[7] = descr.localpos[1];
        v4.tw[7] = descr.localpos[2];
        v4.tu[8] = descr.width_scale;
        v4.tv[8] = descr.height_scale;



    
        meshe->AddVertex( v1 );
        meshe->AddVertex( v2 );
        meshe->AddVertex( v3 );
        meshe->AddVertex( v4 );

        size_t index_base = 4 * i;

        meshe->AddTriangle( Triangle( index_base, 3 + index_base, 1 + index_base ) );
        meshe->AddTriangle( Triangle( 1 + index_base, 3 + index_base, 2 + index_base ) );

    }    
}

bool ImpostorsRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
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

                // initaliser le nuage de quads dans le meshe de chaque passe
                build_quads( pass_slots[i]->getPurpose() );

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

void ImpostorsRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void ImpostorsRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void ImpostorsRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

    if( transform_aspect )
    {
        Matrix world;
        transform_aspect->GetWorldTransform( world );

        Matrix view;
        transform_aspect->GetViewTransform( view );

        Matrix proj;
        transform_aspect->GetProjTransform( proj );

        // redistribution de la transfo world...
        // + redistribution des proj/view distribues

        ComponentList<PassSlot> pass_slots;
        m_owner->GetComponentsByType<PassSlot>( pass_slots );

        for( size_t i = 0; i < pass_slots.size(); i++ )
        {
            pass_slots[i]->getPurpose().m_world = world;
            pass_slots[i]->getPurpose().m_view = view;
            pass_slots[i]->getPurpose().m_proj = proj;
        }
    }
}
