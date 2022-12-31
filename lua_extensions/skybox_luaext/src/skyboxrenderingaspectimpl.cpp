/* -*-LIC_BEGIN-*- */
/*
*
* DrawSpace Rendering engine
* Emmanuel Chaumont Copyright (c) 2013-2023
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

#include "skyboxrenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;

using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;

#define SB_DIM 0.5

SkyboxRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
    m_pass_name( p_pass_name )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;

    Vertex v1, v2, v3, v4;

    for( long i = 0; i < 6; i++ )
    {
        m_meshes[i] = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
        switch( i )
        {
            case FrontQuad:

                v1.x = -SB_DIM;
                v1.y = SB_DIM;
                v1.z = -SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = SB_DIM;
                v2.y = SB_DIM;
                v2.z = -SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = SB_DIM;
                v3.y = -SB_DIM;
                v3.z = -SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -SB_DIM;
                v4.y = -SB_DIM;
                v4.z = -SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox FrontQuad" );

                break;

            case RearQuad:

                v1.x = SB_DIM;
                v1.y = SB_DIM;
                v1.z = SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -SB_DIM;
                v2.y = SB_DIM;
                v2.z = SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -SB_DIM;
                v3.y = -SB_DIM;
                v3.z = SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = SB_DIM;
                v4.y = -SB_DIM;
                v4.z = SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox RearQuad" );
                break;

            case LeftQuad:

                v1.x = -SB_DIM;
                v1.y = SB_DIM;
                v1.z = SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = -SB_DIM;
                v2.y = SB_DIM;
                v2.z = -SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = -SB_DIM;
                v3.y = -SB_DIM;
                v3.z = -SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -SB_DIM;
                v4.y = -SB_DIM;
                v4.z = SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox LeftQuad" );
                break;

            case RightQuad:

                v1.x = SB_DIM;
                v1.y = SB_DIM;
                v1.z = -SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = SB_DIM;
                v2.y = SB_DIM;
                v2.z = SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = SB_DIM;
                v3.y = -SB_DIM;
                v3.z = SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = SB_DIM;
                v4.y = -SB_DIM;
                v4.z = -SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox RightQuad" );
                break;

            case TopQuad:

                v1.x = -SB_DIM;
                v1.y = SB_DIM;
                v1.z = SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = SB_DIM;
                v2.y = SB_DIM;
                v2.z = SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = SB_DIM;
                v3.y = SB_DIM;
                v3.z = -SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -SB_DIM;
                v4.y = SB_DIM;
                v4.z = -SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox TopQuad" );
                break;

            case BottomQuad:

                v1.x = -SB_DIM;
                v1.y = -SB_DIM;
                v1.z = -SB_DIM;
                v1.tu[0] = 0.0;
                v1.tv[0] = 0.0;

                v2.x = SB_DIM;
                v2.y = -SB_DIM;
                v2.z = -SB_DIM;
                v2.tu[0] = 1.0;
                v2.tv[0] = 0.0;

                v3.x = SB_DIM;
                v3.y = -SB_DIM;
                v3.z = SB_DIM;
                v3.tu[0] = 1.0;
                v3.tv[0] = 1.0;

                v4.x = -SB_DIM;
                v4.y = -SB_DIM;
                v4.z = SB_DIM;
                v4.tu[0] = 0.0;
                v4.tv[0] = 1.0;

                m_meshes[i]->SetPath( "Skybox BottomQuad" );
                break;
        }

        m_meshes[i]->AddVertex( v1 );
        m_meshes[i]->AddVertex( v2 );
        m_meshes[i]->AddVertex( v3 );
        m_meshes[i]->AddVertex( v4 );

        m_meshes[i]->AddTriangle( Triangle( 0, 3, 1 ) );
        m_meshes[i]->AddTriangle( Triangle( 1, 3, 2 ) );
    }

    
    m_cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) );

    for( long i = 0; i < 6; i++ )
    {
        m_rendering_node[i] = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
        m_rendering_node[i]->RegisterHandler( m_cb );
        m_rendering_node[i]->SetMeshe( m_meshes[i] );

        m_rendering_node[i]->m_debug_id = "skybox_node";
    }

    m_world.Identity();
    
    m_view.Identity();
    m_proj.Identity();
}

SkyboxRenderingAspectImpl::PassSlot::~PassSlot( void )
{    
    for( long i = 0; i < 6; i++ )
    {
        if (m_rendering_node[i])
        {
            _DRAWSPACE_DELETE_(m_rendering_node[i]);
        }        
        if (m_meshes[i])
        {
            _DRAWSPACE_DELETE_(m_meshes[i]);
        }        
    }    
    if (m_cb)
    {
        _DRAWSPACE_DELETE_(m_cb);
    }    
}       

void SkyboxRenderingAspectImpl::PassSlot::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    m_renderer->DrawMeshe( m_world, m_view, m_proj );
}

SkyboxRenderingAspectImpl::SkyboxRenderingAspectImpl( void )
{
}

bool SkyboxRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    ComponentList<PassSlot> pass_slots;
    m_owner->GetComponentsByType<SkyboxRenderingAspectImpl::PassSlot>(pass_slots);

    for (auto& e : pass_slots)
    {
        auto& pass_slot{ e->getPurpose() };
        if (pass_slot.m_pass_name == p_name)
        {
            if (m_add_in_rendergraph)
            {
                // ajout du renderingnode dans la renderingqueue  
                for (int j = 0; j < 6; j++)
                {
                    p_passqueue->Add(pass_slot.m_rendering_node[j]);
                }
            }
            else
            {
                // suppression du renderingnode de la renderingqueue
                for (int j = 0; j < 6; j++)
                {
                    p_passqueue->Remove(pass_slot.m_rendering_node[j]);
                }
            }
            updated_queue = true;
        }
    }

    return updated_queue;
}

void SkyboxRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void SkyboxRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void SkyboxRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();
    if( transform_aspect )
    {
        Matrix world;        
        transform_aspect->GetWorldTransform( world );
        world.ClearTranslation();

        Matrix view;
        transform_aspect->GetViewTransform( view );
        view.ClearTranslation();

        Matrix proj;
        transform_aspect->GetProjTransform( proj ); 

        ComponentList<PassSlot> pass_slots;
        m_owner->GetComponentsByType<SkyboxRenderingAspectImpl::PassSlot>(pass_slots);

        for (auto& e : pass_slots)
        {
            auto& pass_slot{ e->getPurpose() };
        
            pass_slot.m_world = world;
            pass_slot.m_view = view;
            pass_slot.m_proj = proj;
        }
    }
    ////////////////////////////////////////////////////////

    //update_shader_params();
}
