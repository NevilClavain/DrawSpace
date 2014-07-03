/*
*                                                                          
* DrawSpace Rendering engine                                               
* Emmanuel Chaumont Copyright (c) 2013-2014                                
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
#include "planet_drawing.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planet;


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

void FaceDrawingNode::draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const DrawSpace::Utils::Matrix& p_world, const DrawSpace::Utils::Matrix& p_view, const DrawSpace::Utils::Matrix& p_proj, DrawSpace::Utils::Vector& p_color )
{
    m_renderer->SetFxShaderParams( 1, 0, p_color );

    VSphere* vsphere = p_patch->GetVSphere();

    DrawSpace::Utils::Matrix res;
    res = p_world * p_view;
    vsphere->Transform( res );

    Utils::Vector transformed_vsphere_point;
    vsphere->GetTransformedPoint( transformed_vsphere_point );
    
    bool draw = false;
    if( vsphere->Collide( Utils::Vector( 0.0, 0.0, 0.0, 1.0 ) ) )
    {
        draw = true;
    }
    else
    {
        if( transformed_vsphere_point[2] <= 0.0 )
        {
            draw = true;
        }
    }
    
    if( draw )
    {
        Vector flag0;
        flag0[0] = p_patch->GetOrientation();
        flag0[1] = p_patch->GetSideLength() / p_ray;
        flag0[2] = p_ray;
        SetShaderRealVector( "flag0", flag0 );

        Vector patch_pos;
        dsreal xp, yp;
        p_patch->GetPos( xp, yp );

        patch_pos[0] = xp / p_ray;
        patch_pos[1] = yp / p_ray;
        patch_pos[2] = 0.0;
        SetShaderRealVector( "patch_translation", patch_pos );

        m_renderer->SetFxShaderParams( 0, 8, flag0 );
        m_renderer->SetFxShaderParams( 0, 9, patch_pos );

        m_renderer->DrawMeshe( p_nbv, p_nbt, p_world, p_view, p_proj );
    }       
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
            draw_single_patch( (*it).second, p_nbv, p_nbt, p_ray, p_world, p_view, p_proj, Vector( 0.0, 0.0, 1.0, 0.0 ) );
        }
    }
    else
    {    
        QuadtreeNode<Patch>* current_leaf = m_face->GetCurrentLeaf();

        draw_single_patch( current_leaf->GetContent(), p_nbv, p_nbt, p_ray, p_world, p_view, p_proj, Vector( 0.0, 0.0, 1.0, 0.0 ) );
 
        for( long i = 0; i < 8; i++ )
        {
            QuadtreeNode<Patch>* neighb = static_cast<QuadtreeNode<Patch>*>( current_leaf->GetContent()->GetNeighbour( i ) );

            if( neighb )
            {
                draw_single_patch( neighb->GetContent(), p_nbv, p_nbt, p_ray, p_world, p_view, p_proj, Vector( 0.0, 0.0, 1.0, 0.0 ) );
            }
        }  
    }
}


Drawing::Drawing( void ) :
m_renderer( NULL ),
m_scenegraph( NULL ),
m_planetbody( NULL )
{
    m_fx = _DRAWSPACE_NEW_( Fx, Fx );

    // prepare Fx

    m_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    m_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet.vsh", false ) ) );
    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet.psh", false ) ) );

    m_fx->GetShader( 0 )->LoadFromFile();
    m_fx->GetShader( 1 )->LoadFromFile();
}

Drawing::~Drawing( void )
{
    _DRAWSPACE_DELETE_( m_fx );
}

void Drawing::SetCurrentPlanetBody( Body* p_planetbody )
{
    m_planetbody = p_planetbody;

    // faces update
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
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

void Drawing::OnRegister( DrawSpace::Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "Planet Drawing : pass '";
            msg += (*it).first;
            msg += "' does not exists in scenegraph";

            _DSEXCEPTION( msg )
        }

        for( long i = 0; i < 6; i++ )
        {
            current_pass->GetRenderingQueue()->Add( (*it).second.nodes[i] );
        }
    }
    m_scenegraph = p_scenegraph;
}

void Drawing::on_renderingnode_draw( RenderingNode* p_rendering_node )
{
    if( !m_planetbody )
    {
        return;
    }

    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    m_scenegraph->GetCurrentCameraView( view );
    m_scenegraph->GetCurrentCameraProj( proj );

    FaceRenderingNode* face_node = static_cast<FaceRenderingNode*>( p_rendering_node );
    face_node->Draw( Body::m_planetpatch_meshe->GetVertexListSize(), Body::m_planetpatch_meshe->GetTrianglesListSize(), m_planetbody->m_diameter / 2.0, m_globaltransformation, view, proj );    
}

void Drawing::RegisterPassSlot( const dsstring p_passname )
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
    m_passesnodes[p_passname] = nodeset;
}

DrawSpace::Core::RenderingNode* Drawing::GetNodeFromPass( const dsstring& p_passname, int p_faceid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }
    NodesSet nodeset = m_passesnodes[p_passname];
    
    return nodeset.nodes[p_faceid];
}

void Drawing::SetNodeFromPassSpecificFx( const dsstring& p_passname, int p_faceid, const dsstring& p_fxname )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return;
    }
    NodesSet nodeset = m_passesnodes[p_passname];
    
    if( "main_fx" == p_fxname )
    {
        Fx* fx = nodeset.nodes[p_faceid]->GetFx();
        *fx = *m_fx; 
    }
}

