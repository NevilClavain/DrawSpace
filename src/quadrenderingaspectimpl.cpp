/* -*-LIC_BEGIN-*- */
/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2020                     
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
using namespace DrawSpace::Interface;

QuadRenderingAspectImpl::QuadRenderingAspectImpl( void )
{
}

bool QuadRenderingAspectImpl::Init( DrawSpace::Core::Entity* p_entity, DrawSpace::Utils::TimeManager* p_timemanager)
{
    Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

    ComponentList<Core::RenderingNode*> quads_nodes;
    m_owner->GetComponentsByType<Core::RenderingNode*>( quads_nodes );

    Core::RenderingNode* node = quads_nodes[0]->getPurpose();

    Fx* fx = node->GetFx();

    if( false == renderer->CreateShaders( fx, &m_sh_data ) )
    {
        _DSEXCEPTION( "Cannot create Shaders" )
    }
    
    for( long j = 0; j < node->GetTextureListSize(); j++ )
    {
        Texture* current_tx = node->GetTexture( j );
        if( NULL != current_tx )
        {
            if( false == renderer->CreateTexture( current_tx, &m_tx_data ) )
            {
                dsstring path;
                current_tx->GetPath( path );

                dsstring excp_msg = "Cannot create Texture ";
                excp_msg += path;
                _DSEXCEPTION( excp_msg  )
            }
        }
    }


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

    node->GetMeshe()->ClearTriangles();
    node->GetMeshe()->ClearVertices();

    node->GetMeshe()->AddVertex( v1 );
    node->GetMeshe()->AddVertex( v2 );
    node->GetMeshe()->AddVertex( v3 );
    node->GetMeshe()->AddVertex( v4 );

    node->GetMeshe()->AddTriangle( Triangle( 0, 2, 1 ) );
    node->GetMeshe()->AddTriangle( Triangle( 0, 3, 2 ) );

    if( false == renderer->CreateMeshe( node->GetMeshe(), &m_meshe_data ) )
    {
        _DSEXCEPTION( "Cannot create Meshe" )
    }

    // construire un matrice projection classique
    DrawSpace::Interface::Renderer::Characteristics characteristics;
    renderer->GetRenderCharacteristics( characteristics );
    m_proj.Perspective( characteristics.width_viewport, characteristics.height_viewport, 1.0, 10.0 );


    return true;
}

void QuadRenderingAspectImpl::Run( DrawSpace::Core::Entity* p_entity )
{
    ComponentList<bool> flags;
    m_owner->GetComponentsByType<bool>( flags );
    
    if( false == flags[0]->getPurpose() )
    {
        return;
    }


    TransformAspect* transform_aspect = p_entity->GetAspect<TransformAspect>();

    if( transform_aspect )
    {
        Matrix world;
        transform_aspect->GetWorldTransform( world );

        Renderer* renderer = SingletonPlugin<Renderer>::GetInstance()->m_interface;

        ComponentList<Core::RenderingNode*> quads_nodes;         
        m_owner->GetComponentsByType<Core::RenderingNode*>( quads_nodes );

        Core::RenderingNode* node = quads_nodes[0]->getPurpose();

        Fx* fx = node->GetFx();

        renderer->BeginScreen();

        for( long j = 0; j < node->GetTextureListSize(); j++ )
        {
            Texture* current_tx = node->GetTexture( j );
            if( NULL != current_tx )
            {
                renderer->SetTexture( m_tx_data, j );
            }
        }

        renderer->SetShaders( m_sh_data );
        renderer->SetMeshe( m_meshe_data );
        renderer->ApplyRenderStatesIn( fx );

        std::map<dsstring, RenderingNode::ShadersParams*> node_shaders_params;
        node->GetShadersParams( node_shaders_params );

        for( std::map<dsstring, RenderingNode::ShadersParams*>::iterator it = node_shaders_params.begin(); it != node_shaders_params.end(); ++it )
        {
            RenderingNode::ShadersParams* sp = it->second;

            if( sp->vector )
            {
                renderer->SetFxShaderParams( sp->shader_index, sp->param_register, sp->param_values );
            }
            else
            {
                renderer->SetFxShaderMatrix( sp->shader_index, sp->param_register, sp->mat );
            }
        }

        Matrix view;
        view.Identity();
        renderer->DrawMeshe( world, view, m_proj );

        renderer->ApplyRenderStatesOut( fx );

        for( long j = 0; j < node->GetTextureListSize(); j++ )
        {
            Texture* current_tx = node->GetTexture( j );
            if( NULL != current_tx )
            {
                renderer->UnsetTexture( j );
            }
        }

        renderer->EndScreen();
    }
}
