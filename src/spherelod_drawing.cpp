/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2016                              
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

#include "renderer.h"
#include "plugin.h"
#include "memalloc.h"
#include "exceptions.h"
#include "spherelod_drawing.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::SphericalLOD;


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer, DrawSpace::SphericalLOD::Config* p_config ) :
m_renderer( p_renderer ),
m_current_patch( NULL ),
m_config( p_config )
{
    ZeroMemory( &m_stats, sizeof( Stats ) );
}

FaceDrawingNode::~FaceDrawingNode( void )
{
}

void FaceDrawingNode::SetDisplayList( const std::vector<Patch*>& p_list )
{
    m_display_list = p_list;
}

void FaceDrawingNode::draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, dsreal p_rel_alt, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj )
{
    Vector flag0;
    flag0[0] = p_patch->GetOrientation();
    flag0[1] = p_patch->GetUnitSideLenght();
    flag0[2] = p_ray;

    Vector patch_pos;
    dsreal xp, yp;
    p_patch->GetUnitPos( xp, yp );

    patch_pos[0] = xp;
    patch_pos[1] = yp;
    patch_pos[2] = 0.0;


    Vector uvcoords;
    p_patch->GetUVCoords( uvcoords );


    m_renderer->SetFxShaderParams( 0, 24, flag0 );
    m_renderer->SetFxShaderParams( 0, 25, patch_pos );
    m_renderer->SetFxShaderParams( 0, 26, uvcoords );

    Vector pixels_flags;
    pixels_flags[0] = p_rel_alt;

    m_renderer->SetFxShaderParams( 1, 0, pixels_flags );

    /*
    if( m_current_patch == p_patch )
    {
        m_renderer->SetFxShaderParams( 1, 0, Vector( 0.0, 1.0, 0.0, 1.0 ) );
    }
    else
    {
        m_renderer->SetFxShaderParams( 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) );
    }
    */
                      
    //m_renderer->SetTexture( p_patch->GetTexture( Maps::COLOR_TEXTURE ), 0 );
    //m_renderer->SetVertexTexture( p_patch->GetTexture( Maps::ELEVATION_TEXTURE ), 0 );

    m_renderer->DrawMeshe( p_world, p_view, p_proj );
    m_stats.nb_patchs++;

   // m_renderer->UnsetTexture( 0 );
   // m_renderer->UnsetVertexTexture( 0 );        
}

void FaceDrawingNode::Draw( long p_nbv, long p_nbt, dsreal p_ray, dsreal p_rel_alt, const Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const Matrix& p_proj )
{
    ZeroMemory( &m_stats, sizeof( Stats ) );

    Texture* current_texture = NULL;
        
    for( size_t i = 0; i < m_display_list.size(); i++ )
    {
        Patch* ref_patch = m_display_list[i]->GetTextureReferent();
        Texture* refpatchtexture = ref_patch->GetColorTexture();

        if( refpatchtexture != current_texture )
        {
            m_renderer->SetTexture( refpatchtexture->GetRenderData(), 0 );
            current_texture = refpatchtexture;
        }
        draw_single_patch( m_display_list[i], p_nbv, p_nbt, p_ray, p_rel_alt, p_world, p_view, p_proj );
    }
}

void FaceDrawingNode::GetStats( FaceDrawingNode::Stats& p_stats )
{
    p_stats = m_stats;
}

void FaceDrawingNode::SetCurrentPatch( DrawSpace::SphericalLOD::Patch* p_patch )
{
    m_current_patch = p_patch;
}


Drawing::Drawing( SphericalLOD::Config* p_config ) :
m_renderer( NULL ),
m_planetbody( NULL ),
m_config( p_config )
{
}

Drawing::~Drawing( void )
{
}

void Drawing::SetCurrentPlanetBody( Body* p_planetbody )
{
    m_planetbody = p_planetbody;
}

void Drawing::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}


void Drawing::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{   
    for( auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = it->first;

        NodesSet ns = it->second;
        for( auto it2 = ns.begin(); it2 != ns.end(); ++it2 )
        {
            current_pass->GetRenderingQueue()->Add( it2->first );
        }
    }
    m_scenenodegraph = p_scenegraph;
}

void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( !m_planetbody )
    {
        return;
    }

    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    if( m_scenenodegraph )
    {
        m_scenenodegraph->GetCurrentCameraView( view );
        m_scenenodegraph->GetCurrentCameraProj( proj );
    }

    FaceDrawingNode* face_node = static_cast<FaceDrawingNode*>( p_rendering_node );

    std::vector<Patch*> dl;
 
    m_planetbody->GetFace( m_nodes[face_node] )->GetDisplayList( dl );
    Patch* current_patch = m_planetbody->GetFace( m_nodes[face_node] )->GetCurrentPatch();
 
    face_node->SetCurrentPatch( current_patch );
    face_node->SetDisplayList( dl );

    m_config->m_landscape->BindShadersParams();
    m_config->m_landscape->BindTextures();

    // recup relative alt de la face
    dsreal rel_alt = m_planetbody->GetFace( m_nodes[face_node] )->GetRelativeAlt();

    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), m_planetbody->GetDiameter() / 2.0, rel_alt, m_globaltransformation, view, proj );
    m_config->m_landscape->UnbindTextures();
}

void Drawing::on_rendering_singlenode_draw( DrawSpace::Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix world;
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    world.Translation( 0.0, 0.0, -1.0 );
    view.Identity();
    proj.Perspective( 2.0, 2.0, 1.0, 10.0 );

    FaceDrawingNode* face_node = static_cast<FaceDrawingNode*>( p_rendering_node ); 
    face_node->SetCurrentPatch( NULL );

    m_config->m_landscape->BindShadersParams();
    m_config->m_landscape->BindTextures();

    // recup relative alt de la face
    dsreal rel_alt = m_planetbody->GetFace( m_nodes[face_node] )->GetRelativeAlt();

    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), 1.0, rel_alt, world, view, proj );
    m_config->m_landscape->UnbindTextures();
}

void Drawing::RegisterPlanetBodyPassSlot( Pass* p_pass )
{
    for( long i = 0; i < 6; i++ )
    {   
        FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config ) );
        node->SetMeshe( Body::m_planetpatch_meshe );
        //node->SetTexture( )

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_renderingnode_draw ) );
        node->RegisterHandler( cb );
      
        m_callbacks.push_back( cb );

        m_passesnodes[p_pass][node] = i;
        m_nodes[node] = i;
    }
}

void Drawing::SetSinglePassSlot( Pass* p_pass, SphericalLOD::FaceDrawingNode* p_node )
{
    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_rendering_singlenode_draw ) );
    p_node->RegisterHandler( cb );      
    m_callbacks.push_back( cb );

    m_passes_singlenodes[p_pass] = p_node;
   
    // ces nodes ne sont pas destines a dependre d'un scenegraph
    // donc on ajoute le node a la queue directement ici
    p_pass->GetRenderingQueue()->Add( p_node );
}

DrawSpace::Core::RenderingNode* Drawing::GetPlanetBodyNodeFromPass( Pass* p_pass, int p_faceid )
{
    if( 0 == m_passesnodes.count( p_pass ) )
    {
        return NULL;
    }
    NodesSet nodeset = m_passesnodes[p_pass];
    for( auto it = m_passesnodes[p_pass].begin(); it != m_passesnodes[p_pass].end(); ++it )
    {
        if( it->second == p_faceid )
        {
            return it->first;
        }
    }
    return NULL;
}

DrawSpace::Core::RenderingNode* Drawing::GetSingleNodeFromPass( Pass* p_pass )
{
    if( 0 == m_passes_singlenodes.count( p_pass ) )
    {
        return NULL;
    }
    return m_passes_singlenodes[p_pass];
}

DrawSpace::SphericalLOD::Body* Drawing::GetBody( void )
{
    return m_planetbody;
}

void Drawing::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}

