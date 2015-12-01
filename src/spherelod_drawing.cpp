/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2015                              
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

void FaceDrawingNode::draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj )
{
    Vector flag0;
    flag0[0] = p_patch->GetOrientation();
    flag0[1] = p_patch->GetUnitSideLenght();
    flag0[2] = p_ray;
    flag0[3] = m_config->m_amplitude;

    Vector patch_pos;
    dsreal xp, yp;
    p_patch->GetUnitPos( xp, yp );

    patch_pos[0] = xp;
    patch_pos[1] = yp;
    patch_pos[2] = 0.0;


    Vector uvcoords;
    p_patch->GetUVCoords( uvcoords );

    Vector fbm_params;
    fbm_params[0] = m_config->m_fbmLacunarity;
    fbm_params[1] = m_config->m_fbmInputHalfRange;
    fbm_params[2] = ( m_config->m_fbmClamp ? 1.0 : 0.0 );

    Vector fbm_params2;
    fbm_params2[0] = m_config->m_fbmClipMode;
    fbm_params2[1] = m_config->m_fbmClipValue;
    fbm_params2[2] = m_config->m_fbmRoughness;


    m_renderer->SetFxShaderParams( 0, 24, flag0 );
    m_renderer->SetFxShaderParams( 0, 25, patch_pos );
    m_renderer->SetFxShaderParams( 0, 26, uvcoords );
    m_renderer->SetFxShaderParams( 0, 27, fbm_params );
    m_renderer->SetFxShaderParams( 0, 28, fbm_params2 );

    if( m_current_patch == p_patch )
    {
        m_renderer->SetFxShaderParams( 1, 0, Vector( 0.0, 1.0, 0.0, 1.0 ) );
    }
    else
    {
        m_renderer->SetFxShaderParams( 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) );
    }
                      
    //m_renderer->SetTexture( p_patch->GetTexture( Maps::COLOR_TEXTURE ), 0 );
    //m_renderer->SetVertexTexture( p_patch->GetTexture( Maps::ELEVATION_TEXTURE ), 0 );

    m_renderer->DrawMeshe( p_world, p_view, p_proj );
    m_stats.nb_patchs++;

   // m_renderer->UnsetTexture( 0 );
   // m_renderer->UnsetVertexTexture( 0 );        
}

void FaceDrawingNode::Draw( long p_nbv, long p_nbt, dsreal p_ray, const Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const Matrix& p_proj )
{
    ZeroMemory( &m_stats, sizeof( Stats ) );
    
    for( size_t i = 0; i < m_display_list.size(); i++ )
    {
        draw_single_patch( m_display_list[i], p_nbv, p_nbt, p_ray, p_world, p_view, p_proj );
    }
}
/*
void FaceDrawingNode::CreateNoisingTextures( void )
{
    m_perlinnoisebuffer_texture = new Texture();    
    m_perlinnoisebuffer_texture->SetFormat( 256, 3, 4 );
    m_perlinnoisebuffer_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    m_perlinnoisemap_texture = new Texture();
    m_perlinnoisemap_texture->SetFormat( 256, 1, 4 );
    m_perlinnoisemap_texture->SetPurpose( Texture::PURPOSE_FLOAT );

    SetVertexTexture( m_perlinnoisebuffer_texture, 0 );
    SetVertexTexture( m_perlinnoisemap_texture, 1 );
}

void FaceDrawingNode::InitNoisingTextures( DrawSpace::Utils::Fractal* p_fractal )
{
    m_fractal = p_fractal;

    m_perlinnoisebuffer_texture->AllocTextureContent();
    m_pnbufftexture_content = m_perlinnoisebuffer_texture->GetTextureContentPtr();

    m_perlinnoisemap_texture->AllocTextureContent();
    m_pnmaptexture_content = m_perlinnoisemap_texture->GetTextureContentPtr();
    
    unsigned char* color_ptr = (unsigned char*)m_pnmaptexture_content;
    float* float_ptr = (float*)m_pnbufftexture_content;
        
    for(long j = 0; j < 3; j++ )
    {
        for( long i = 0; i < 256; i++ )    
        {
            float temp = p_fractal->GetNBuffer( i, j );
            *float_ptr = temp; float_ptr++;
        }
    }

    float_ptr = (float*)m_pnmaptexture_content;

    for( long i = 0; i < 256; i++ )
    {
        *float_ptr = p_fractal->GetNMap( i ); float_ptr++;
    }

    m_perlinnoisemap_texture->UpdateTextureContent();
    m_perlinnoisebuffer_texture->UpdateTextureContent();
}
*/

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
 
    m_planetbody->m_faces[m_nodes[face_node]]->GetDisplayList( dl );
    Patch* current_patch = m_planetbody->m_faces[m_nodes[face_node]]->GetCurrentPatch();
 
    face_node->SetCurrentPatch( current_patch );
    face_node->SetDisplayList( dl );
    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), m_planetbody->m_diameter / 2.0, m_globaltransformation, view, proj );

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
    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), 1.0, world, view, proj );

}

void Drawing::RegisterPlanetBodyPassSlot( Pass* p_pass )
{
    for( long i = 0; i < 6; i++ )
    {   
        FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config ) );
        node->SetMeshe( Body::m_planetpatch_meshe );

        //node->CreateNoisingTextures();
        for( long j = 0; j < 6; j++ )
        {
            node->SetVertexTexture( m_perlinnoise_textures_set.textures[j], j );
        }

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_renderingnode_draw ) );
        node->RegisterHandler( cb );
      
        m_callbacks.push_back( cb );

        m_passesnodes[p_pass][node] = i;
        m_nodes[node] = i;
    }
}

void Drawing::RegisterSinglePassSlot( Pass* p_pass )
{
    FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer, m_config ) );
    node->SetMeshe( Body::m_planetpatch_meshe );

    //node->CreateNoisingTextures();
    for( long j = 0; j < 6; j++ )
    {
        node->SetVertexTexture( m_perlinnoise_textures_set.textures[j], j );
    }

    RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_rendering_singlenode_draw ) );
    node->RegisterHandler( cb );      
    m_callbacks.push_back( cb );

    m_passes_singlenodes[p_pass] = node;

    std::vector<Patch*> dl;
   
    // ces nodes ne sont pas destines a dependre d'un scenegraph
    // donc on ajoute le node a la queue directement ici
    p_pass->GetRenderingQueue()->Add( node );
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
/*
void Drawing::InitNoisingTextures( DrawSpace::Utils::Fractal* p_fractal )
{    
    for( auto it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        NodesSet ns = it->second;
        for( auto it2 = ns.begin(); it2 != ns.end(); ++it2 )
        {
            it2->first->InitNoisingTextures( p_fractal );
        }
    }

    for( auto it = m_passes_singlenodes.begin(); it != m_passes_singlenodes.end(); ++it )
    {
        it->second->InitNoisingTextures( p_fractal );
    }
}
*/

void Drawing::SetPerlinNoiseTextureSet( PerlinNoiseTexturesSet& p_textures )
{
    m_perlinnoise_textures_set = p_textures;
}