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


FaceDrawingNode::FaceDrawingNode( DrawSpace::Interface::Renderer* p_renderer ) :
m_renderer( p_renderer ),
m_face( NULL )
{

}

FaceDrawingNode::~FaceDrawingNode( void )
{


}

void FaceDrawingNode::SetFace( Face* p_face )
{
    m_face = p_face;
}

void FaceDrawingNode::draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj )
{    
    Vector flag0;
    flag0[0] = p_patch->GetOrientation();
    flag0[1] = p_patch->GetUnitSideLenght();
    flag0[2] = p_ray;

    Vector patch_pos;
    dsreal xp, yp;
    p_patch->GetUnitPos( xp, yp );

    Vector uvcoords;
    p_patch->GetUVCoords( uvcoords );

    patch_pos[0] = xp;
    patch_pos[1] = yp;
    patch_pos[2] = 0.0;

    m_renderer->SetFxShaderParams( 0, 24, flag0 );
    m_renderer->SetFxShaderParams( 0, 25, patch_pos );
    m_renderer->SetFxShaderParams( 0, 26, uvcoords );

    m_renderer->SetFxShaderParams( 1, 0, Vector( 1.0, 1.0, 1.0, 1.0 ) );
                      
    //m_renderer->SetTexture( p_patch->GetTexture( Maps::COLOR_TEXTURE ), 0 );
    //m_renderer->SetVertexTexture( p_patch->GetTexture( Maps::ELEVATION_TEXTURE ), 0 );

    m_renderer->DrawMeshe( p_world, p_view, p_proj );

   // m_renderer->UnsetTexture( 0 );
   // m_renderer->UnsetVertexTexture( 0 );        
}

void FaceDrawingNode::Draw( long p_nbv, long p_nbt, dsreal p_ray, const Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const Matrix& p_proj )
{
    if( !m_face )
    {
        return;
    }

    long currentleaf_depth = -1;   
    if( m_face->GetCurrentLeaf() )
    {
        currentleaf_depth = m_face->GetCurrentLeaf()->GetDepthLevel();
    }
    
    if( -1 == currentleaf_depth || currentleaf_depth < 3 )
    {
        std::map<dsstring, Patch*> patchesleafs;
        m_face->GetLeafs( patchesleafs );

        for( std::map<dsstring, Patch*>::iterator it = patchesleafs.begin(); it != patchesleafs.end(); ++it )
        {
            draw_single_patch( (*it).second, p_nbv, p_nbt, p_ray, p_world, p_view, p_proj );
        }
    }
    else
    {    
        QuadtreeNode<Patch>* current_leaf = m_face->GetCurrentLeaf();

        draw_single_patch( current_leaf->GetContent(), p_nbv, p_nbt, p_ray, p_world, p_view, p_proj );
 
        for( long i = 0; i < 8; i++ )
        {
            QuadtreeNode<Patch>* neighb = static_cast<QuadtreeNode<Patch>*>( current_leaf->GetContent()->GetNeighbour( i ) );

            if( neighb )
            {
                draw_single_patch( neighb->GetContent(), p_nbv, p_nbt, p_ray, p_world, p_view, p_proj );
            }
        }  
    }
}


Drawing::Drawing( void ) :
m_renderer( NULL ),
m_planetbody( NULL )
{
    /*
    m_fx = _DRAWSPACE_NEW_( Fx, Fx );

    // prepare Fx

    m_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "true" ) );
    m_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "linear" ) );
    m_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    m_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::ENABLEZBUFFER, "false" ) );
    m_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETTEXTUREFILTERTYPE, "none" ) );
    m_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet2.vsh", false ) ) );
    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet2.psh", false ) ) );

    m_fx->GetShader( 0 )->LoadFromFile();
    m_fx->GetShader( 1 )->LoadFromFile();
    */
}

Drawing::~Drawing( void )
{
    //_DRAWSPACE_DELETE_( m_fx );
}

void Drawing::SetCurrentPlanetBody( Body* p_planetbody )
{
    m_planetbody = p_planetbody;

    // faces update
    for( std::map<Pass*, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        for( long i = 0; i < 6; i++ )
        {
            it->second.nodes[i]->SetFace( m_planetbody->m_faces[i] );
        }
    }
}

void Drawing::SetRenderer( DrawSpace::Interface::Renderer* p_renderer )
{
    m_renderer = p_renderer;
}


void Drawing::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{   
    for( std::map<Pass*, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = it->first;

        for( long i = 0; i < 6; i++ )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
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
    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), m_planetbody->m_diameter / 2.0, m_globaltransformation, view, proj );
}

void Drawing::RegisterPassSlot( Pass* p_pass )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {            
        nodeset.nodes[i] = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( m_renderer ) );
        nodeset.nodes[i]->SetMeshe( Body::m_planetpatch_meshe );

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Drawing::on_renderingnode_draw ) );
        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );
    }

    m_passesnodes[p_pass] = nodeset;
}

DrawSpace::Core::RenderingNode* Drawing::GetNodeFromPass( Pass* p_pass, int p_faceid )
{
    if( 0 == m_passesnodes.count( p_pass ) )
    {
        return NULL;
    }
    NodesSet nodeset = m_passesnodes[p_pass];

    return nodeset.nodes[p_faceid];
}


DrawSpace::SphericalLOD::Body* Drawing::GetBody( void )
{
    return m_planetbody;
}

void Drawing::SetFinalTransform( const DrawSpace::Utils::Matrix& p_mat )
{
    m_globaltransformation = p_mat;
}