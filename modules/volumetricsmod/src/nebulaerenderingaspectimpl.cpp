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


#include "nebulaerenderingaspectimpl.h"
#include "renderingaspect.h"
#include "transformaspect.h"


using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Aspect;
using namespace DrawSpace::AspectImplementations;
using namespace DrawSpace::RenderGraph;
using namespace DrawSpace::Utils;


NebulaeRenderingAspectImpl::PassSlot::PassSlot( const dsstring& p_pass_name ) :
    m_pass_name( p_pass_name )
{
    m_renderer = DrawSpace::Core::SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;


    // BUILD MESHE HERE

    Vertex v1, v2, v3, v4;

    m_meshe = _DRAWSPACE_NEW_(Core::Meshe, Core::Meshe);

    v1.x = -0.5;
    v1.y = 0.5;
    v1.z = 0.0;
    v1.tu[0] = 0.0;
    v1.tv[0] = 0.0;

    v2.x = 0.5;
    v2.y = 0.5;
    v2.z = 0.0;
    v2.tu[0] = 1.0;
    v2.tv[0] = 0.0;

    v3.x = 0.5;
    v3.y = -0.5;
    v3.z = 0.0;
    v3.tu[0] = 1.0;
    v3.tv[0] = 1.0;

    v4.x = -0.5;
    v4.y = -0.5;
    v4.z = 0.0;
    v4.tu[0] = 0.0;
    v4.tv[0] = 1.0;

    m_meshe->AddVertex(v1);
    m_meshe->AddVertex(v2);
    m_meshe->AddVertex(v3);
    m_meshe->AddVertex(v4);

    m_meshe->AddTriangle(Triangle(0, 3, 1));
    m_meshe->AddTriangle(Triangle(1, 3, 2));

    /////////////////
    
    m_cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &PassSlot::on_renderingnode_draw ) );

    m_rendering_node = _DRAWSPACE_NEW_( RenderingNode, RenderingNode );
    m_rendering_node->RegisterHandler( m_cb );
    m_rendering_node->SetMeshe( m_meshe );

    m_world.Identity();
    
    m_view.Identity();
    m_proj.Identity();
}

NebulaeRenderingAspectImpl::PassSlot::~PassSlot( void )
{    
    _DRAWSPACE_DELETE_( m_rendering_node );
    _DRAWSPACE_DELETE_( m_meshe );
    _DRAWSPACE_DELETE_(m_cb);
}       

void NebulaeRenderingAspectImpl::PassSlot::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    m_renderer->DrawMeshe( m_world, m_view, m_proj );
}

NebulaeRenderingAspectImpl::NebulaeRenderingAspectImpl( void )
{
}

bool NebulaeRenderingAspectImpl::VisitRenderPassDescr( const dsstring& p_name, DrawSpace::Core::RenderingQueue* p_passqueue )
{
    bool updated_queue = false;

    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        if( m_pass_slots[i]->m_pass_name == p_name )
        {
            if( m_add_in_rendergraph )
            {
                // ajout du renderingnode dans la renderingqueue  
                p_passqueue->Add( m_pass_slots[i]->m_rendering_node );
            }
            else
            {
                // suppression du renderingnode de la renderingqueue
                p_passqueue->Remove( m_pass_slots[i]->m_rendering_node );
            }
            updated_queue = true;
        }
    }
    return updated_queue;
}

void NebulaeRenderingAspectImpl::RegisterToRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    init_rendering_objects();

    m_add_in_rendergraph = true;
    p_rendergraph.Accept( this );
}

void NebulaeRenderingAspectImpl::UnregisterFromRendering( DrawSpace::RenderGraph::RenderPassNodeGraph& p_rendergraph )
{
    release_rendering_objects();

    m_add_in_rendergraph = false;
    p_rendergraph.Accept( this );
}

void NebulaeRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
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

        for( size_t i = 0; i < m_pass_slots.size(); i++ )
        {
            m_pass_slots[i]->m_world = world;
            m_pass_slots[i]->m_view = view;
            m_pass_slots[i]->m_proj = proj;
        }
    }

    update_shader_params();

    ////////////////////////////////////////////////////////
}

void NebulaeRenderingAspectImpl::init_rendering_objects( void )
{
    ComponentList<std::vector<dsstring>> passes;
    m_owner->GetComponentsByType<std::vector<dsstring>>( passes );

    ComponentList<std::array<Texture*,RenderingNode::NbMaxTextures>> textures;
    m_owner->GetComponentsByType<std::array<Texture*,RenderingNode::NbMaxTextures>>( textures );

    ComponentList<Fx*> fxs;
    m_owner->GetComponentsByType<Fx*>( fxs );

    ComponentList<int> ro;
    m_owner->GetComponentsByType<int>( ro );

    std::vector<dsstring> passes_names = passes[0]->getPurpose();
    
    for( size_t i = 0; i < passes_names.size(); i++ )
    {        
        dsstring pass_name;
        pass_name = passes_names[i];
       
        PassSlot* pass_slot = _DRAWSPACE_NEW_( PassSlot, PassSlot( pass_name ) );

        pass_slot->GetRenderingNode()->SetOrderNumber( ro[i]->getPurpose() );
        pass_slot->GetRenderingNode()->SetFx( fxs[i]->getPurpose() );

        std::array<Texture*,RenderingNode::NbMaxTextures> textures_set = textures[i]->getPurpose();

        for( size_t k = 0; k < RenderingNode::NbMaxTextures; k++ )
        {
            pass_slot->GetRenderingNode()->SetTexture( textures_set[k], k );
        }

        m_pass_slots.push_back( pass_slot );
    }
    
    update_shader_params();   
}

void NebulaeRenderingAspectImpl::release_rendering_objects( void )
{
    for( size_t i = 0; i < m_pass_slots.size(); i++ )
    {
        _DRAWSPACE_DELETE_( m_pass_slots[i] );
    }
    m_pass_slots.clear();
}

void NebulaeRenderingAspectImpl::update_shader_params( void ) // for all passes
{
    ////////////////////////////////////////////////////////
    //recup des params shaders

    ComponentList<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>> shaders_params;

    m_owner->GetComponentsByType<std::vector<std::pair<dsstring, RenderingNode::ShadersParams>>>( shaders_params );

    for( size_t i = 0; i < shaders_params.size(); i++ )
    {
        // pour chaque passe
        PassSlot* curr_pass = m_pass_slots[i];

        std::vector<std::pair<dsstring, RenderingNode::ShadersParams>> pass_shaders_params = shaders_params[i]->getPurpose();

        for( size_t k = 0; k < shaders_params.size(); k++ )
        {
            std::pair<dsstring, RenderingNode::ShadersParams> shader_params_pair = pass_shaders_params[k];
            curr_pass->GetRenderingNode()->UpdateShaderParams( shader_params_pair.first, shader_params_pair.second );
        }
    }
}
