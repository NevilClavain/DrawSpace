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
#include "quadtree.h"
#include "exceptions.h"
#include "planet_body.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planet;


FaceRenderingNode::FaceRenderingNode( Face* p_face, DrawSpace::Interface::Renderer* p_renderer ) : 
m_renderer( p_renderer ),
m_face( p_face )
{
    m_patchinstanciationcallback = _DRAWSPACE_NEW_( PatchInstanciationCallback, PatchInstanciationCallback( this, &FaceRenderingNode::on_patchinstanciation ) );
    m_patchdelcallback = _DRAWSPACE_NEW_( PatchDelCallback, PatchDelCallback( this, &FaceRenderingNode::on_patchdel ) );
    m_patchsplitcallback = _DRAWSPACE_NEW_( PatchSplitCallback, PatchSplitCallback( this, &FaceRenderingNode::on_patchsplit ) );
    m_patchmergecallback = _DRAWSPACE_NEW_( PatchMergeCallback, PatchMergeCallback( this, &FaceRenderingNode::on_patchmerge ) );

    m_face->AddInstHandler( GetPatchInstanciationHandler() );
    m_face->AddDelHandler( GetPatchDelHandler() );
    m_face->AddSplitHandler( GetPatchSplitHandler() );
    m_face->AddMergeHandler( GetPatchMergeHandler() );    
}

FaceRenderingNode::~FaceRenderingNode( void )
{
}

void FaceRenderingNode::draw_single_patch( Patch* p_patch, long p_nbv, long p_nbt, dsreal p_ray, const Matrix& p_world, const Matrix& p_view, const Matrix& p_proj, Vector& p_color )
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

void FaceRenderingNode::Draw( long p_nbv, long p_nbt, dsreal p_ray, const Matrix& p_world, const Matrix& p_view, const Matrix& p_proj )
{
    long currentleaf_depth = -1;   
    if( m_face->GetCurrentLeaf() )
    {
        currentleaf_depth = m_face->GetCurrentLeaf()->GetDepthLevel();
    }

    
    if( -1 == currentleaf_depth || currentleaf_depth < 3 )
    {
        for( std::map<dsstring, Patch*>::iterator it = m_patchesleafs.begin(); it != m_patchesleafs.end(); ++it )
        {
            draw_single_patch( (*it).second, p_nbv, p_nbt, p_ray, p_world, p_view, p_proj, Vector( 1.0, 1.0, 1.0, 0.0 ) );
        }
    }
    else
    {
    
        QuadtreeNode<Patch>* current_leaf = m_face->GetCurrentLeaf();

        draw_single_patch( current_leaf->GetContent(), p_nbv, p_nbt, p_ray, p_world, p_view, p_proj, Vector( 1.0, 1.0, 1.0, 0.0 ) );
 
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

void FaceRenderingNode::on_patchinstanciation( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

    m_patchesleafs[patch_name] = p_patch;
    m_patches[patch_name] = p_patch;
}

void FaceRenderingNode::on_patchdel( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
    if( m_patches.count( patch_name ) > 0 )
    {
        m_patches.erase( patch_name );
    }
}

void FaceRenderingNode::on_patchsplit( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );
    if( m_patchesleafs.count( patch_name ) > 0 )
    {
        m_patchesleafs.erase( patch_name );
    }
}

void FaceRenderingNode::on_patchmerge( int p_orientation, Patch* p_patch )
{
    dsstring patch_name;
    p_patch->GetName( patch_name );

    m_patchesleafs[patch_name] = p_patch;
}

Face::PatchInstanciationHandler* FaceRenderingNode::GetPatchInstanciationHandler( void )
{
    return m_patchinstanciationcallback;
}

Face::PatchDeletionHandler* FaceRenderingNode::GetPatchDelHandler( void )
{
    return m_patchdelcallback;
}

Face::PatchSplitHandler* FaceRenderingNode::GetPatchSplitHandler( void )
{
    return m_patchsplitcallback;
}

Face::PatchMergeHandler* FaceRenderingNode::GetPatchMergeHandler( void )
{
    return m_patchmergecallback;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Body::Body( dsreal p_diameter ) : 
m_renderer( NULL ), 
m_scenegraph( NULL ),
/*
m_diameter( "diameter" ),
m_hotpoint( "hotpoint" ),
m_relative_hotpoint( "relative_hotpoint" ),
m_altitud( "altitud" ),
m_split( "split" ),
*/
//m_evt_handler( NULL ),
m_diameter( p_diameter )
{
    m_patchmeshe = _DRAWSPACE_NEW_( Core::Meshe, Core::Meshe );
    build_patch();

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i] = _DRAWSPACE_NEW_( Face, Face );
        m_faces[i]->SetPlanetDiameter( m_diameter );
    }

    /*
    m_diameter.m_value = 10.0;
    m_altitud.m_value = -1.0;
    */

    m_fx = _DRAWSPACE_NEW_( Fx, Fx );

    // prepare Fx

    m_fx->AddRenderStateIn( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "line" ) );
    m_fx->AddRenderStateOut( DrawSpace::Core::RenderState( DrawSpace::Core::RenderState::SETFILLMODE, "solid" ) );

    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet.vsh", false ) ) );
    m_fx->AddShader( _DRAWSPACE_NEW_( Shader, Shader( "planet.psh", false ) ) );

    m_fx->GetShader( 0 )->LoadFromFile();
    m_fx->GetShader( 1 )->LoadFromFile();

}

Body::~Body( void )
{
    _DRAWSPACE_DELETE_( m_patchmeshe );
    _DRAWSPACE_DELETE_( m_fx );

    for( long i = 0; i < 6; i++ )
    {
        _DRAWSPACE_DELETE_( m_faces[i] );
    }
}

void Body::Initialize( void )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->Init( i );
    }
}

void Body::SetRenderer( DrawSpace::Interface::Renderer * p_renderer )
{
    m_renderer = p_renderer;
}

void Body::OnRegister( DrawSpace::Scenegraph* p_scenegraph )
{
    for( std::map<dsstring, NodesSet>::iterator it = m_passesnodes.begin(); it != m_passesnodes.end(); ++it )
    {
        Pass* current_pass = p_scenegraph->GetPass( (*it).first );

        if( NULL == current_pass )
        {
            dsstring msg = "PlanetBuild : pass '";
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


void Body::on_renderingnode_draw( Core::RenderingNode* p_rendering_node )
{
    DrawSpace::Utils::Matrix view;
    DrawSpace::Utils::Matrix proj;

    m_scenegraph->GetCurrentCameraView( view );
    m_scenegraph->GetCurrentCameraProj( proj );

    FaceRenderingNode* face_node = static_cast<FaceRenderingNode*>( p_rendering_node );
    face_node->Draw( m_patchmeshe->GetVertexListSize(), m_patchmeshe->GetTrianglesListSize(), m_diameter / 2.0, m_globaltransformation, view, proj );
}

void Body::RegisterPassSlot( const dsstring p_passname )
{
    NodesSet nodeset;
    for( long i = 0; i < 6; i++ )
    {            
        nodeset.nodes[i] = _DRAWSPACE_NEW_( FaceRenderingNode, FaceRenderingNode( m_faces[i], m_renderer ) );

        nodeset.nodes[i]->SetMeshe( m_patchmeshe );

        RenderingNodeDrawCallback* cb = _DRAWSPACE_NEW_( RenderingNodeDrawCallback, RenderingNodeDrawCallback( this, &Body::on_renderingnode_draw ) );
        nodeset.nodes[i]->RegisterHandler( cb );
        m_callbacks.push_back( cb );

    }
    m_passesnodes[p_passname] = nodeset;
}

DrawSpace::Core::RenderingNode* Body::GetNodeFromPass( const dsstring& p_passname, int p_faceid )
{
    if( 0 == m_passesnodes.count( p_passname ) )
    {
        return NULL;
    }
    NodesSet nodeset = m_passesnodes[p_passname];
    
    return nodeset.nodes[p_faceid];
}

void Body::Compute( void )
{
    // determiner la "face courante";

    int curr_face = 0;
    dsreal af = m_faces[0]->GetAlignmentFactor();

    for( long i = 1; i < 6; i++ )
    {
        if( m_faces[i]->GetAlignmentFactor() > af )
        {
            curr_face = i;
            af = m_faces[i]->GetAlignmentFactor();
        }
    }

    m_current_face = curr_face;

    //////////////////////////////////////

    bool status = 0;

    status = m_faces[Patch::FrontPlanetFace]->Compute() | status;
    status = m_faces[Patch::RearPlanetFace]->Compute() | status;
    status = m_faces[Patch::TopPlanetFace]->Compute() | status; 
    status = m_faces[Patch::BottomPlanetFace]->Compute() | status;
    status = m_faces[Patch::RightPlanetFace]->Compute() | status;
    status = m_faces[Patch::LeftPlanetFace]->Compute() | status;

    /*
    if( status && m_evt_handler )
    {
        (*m_evt_handler)( m_current_face );
    }
    */

    if( status )
    {
        for( std::vector<EventHandler*>::iterator it = m_evt_handlers.begin(); it != m_evt_handlers.end(); ++it )
        {
            ( **it )( this, m_current_face );
        }
    }
}

void Body::SetNodeFromPassSpecificFx( const dsstring& p_passname, int p_faceid, const dsstring& p_fxname )
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

        //nodeset.nodes[p_faceid]->AddShaderParameter( 1, "color", 0 );
        //nodeset.nodes[p_faceid]->SetShaderRealVector( "color", Vector( 0.0, 0.0, 1.0, 0.0 ) );
    }
}

void Body::build_patch( void )
{
    dsreal xcurr, ycurr;
    long patch_resolution = Planet::Patch::Resolution;

    // on travaille sur une sphere de rayon = 1.0, donc diametre = 2.0
    dsreal interval = 2.0 / ( patch_resolution - 1 );
    for( long i = 0; i < patch_resolution; i++ )
    {
        for( long j = 0; j < patch_resolution; j++ )
        {
            xcurr = j * interval - 1.0;
            ycurr = i * interval - 1.0;
                        
            Vertex vertex;
            vertex.x = xcurr;
            vertex.y = ycurr;
            vertex.z = 0.0;
            m_patchmeshe->AddVertex( vertex );
        }
    }

    long current_index = 0;

    for( long i = 0; i < patch_resolution - 1; i++  )
    {
        current_index = i * patch_resolution;

        for( long j = 0; j < patch_resolution - 1; j++ )
        {
            Triangle triangle;

            triangle.vertex1 = current_index;
            triangle.vertex2 = current_index + 1;
            triangle.vertex3 = current_index + patch_resolution;
            m_patchmeshe->AddTriangle( triangle );
            
            triangle.vertex1 = current_index + 1;
            triangle.vertex2 = current_index + 1 + patch_resolution;
            triangle.vertex3 = current_index + patch_resolution;
            m_patchmeshe->AddTriangle( triangle );
            
            current_index++;
        }        
    }
}

void Body::RegisterEventHandler( EventHandler* p_handler )
{
    //m_evt_handler = p_handler;
    m_evt_handlers.push_back( p_handler );
}

void Body::UpdateHotPoint( const DrawSpace::Utils::Vector& p_hotpoint )
{
    m_hotpoint = p_hotpoint;

    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->UpdateRelativeHotpoint( m_hotpoint );
    }

    // compute altitud
    m_altitud = m_hotpoint.Length() - ( m_diameter / 2.0 );
}

DrawSpace::Core::Meshe* Body::GetPatcheMeshe( void )
{
    return m_patchmeshe;
}

Patch* Body::GetFaceCurrentLeaf( int p_faceid )
{
    QuadtreeNode<Patch>* current_leaf = m_faces[p_faceid]->GetCurrentLeaf();
    return current_leaf->GetContent();
}

dsreal Body::GetAltitud( void )
{
    return m_altitud;
}

void Body::ResetMeshes( void )
{
    for( long i = 0; i < 6; i++ )
    {
        m_faces[i]->ResetMeshe();
    }
}