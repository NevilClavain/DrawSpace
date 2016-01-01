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

#include "planetoid_fragment.h"
#include "planetoid_body.h"
#include "spherelod_drawing.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planetoid;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::SphericalLOD;

Fragment::Fragment( DrawSpace::SphericalLOD::Config* p_config, DrawSpace::Dynamics::World* p_world, DrawSpace::SphericalLOD::Body* p_planetbody, 
                Collider* p_collider, dsreal p_planetray, bool p_collisions, Fragment::SubPassCreationHandler* p_handler ) :
m_world( p_world ),
m_config( p_config ),
m_planetbody( p_planetbody ), 
m_collider( p_collider ),
m_collision_state( false ),
m_planetray( p_planetray ),
m_hot( false ),
m_camera( NULL ),
m_inertbody( NULL ),
m_collisions( p_collisions ),
m_nb_collisionmeshebuild_done( 0 ),
m_current_patch( NULL ),
m_current_patch_lod( -1 ),
//m_collidinghm_subpassindex( -1 ),
m_collidingheightmap_texture( NULL ),
m_collidingheightmap_content( NULL ),
m_draw_collidinghm( false )
{
    if( m_collisions )
    {
        m_patch_update_cb = _DRAWSPACE_NEW_( PatchUpdateCb, PatchUpdateCb( this, &Fragment::on_patchupdate ) );
        m_planetbody->RegisterPatchUpdateHandler( m_patch_update_cb );

        m_collidingheightmap_pass = create_colliding_heightmap_pass();

        // creation/preparation du node

        DrawSpace::Interface::Renderer* renderer = SingletonPlugin<DrawSpace::Interface::Renderer>::GetInstance()->m_interface;
        FaceDrawingNode* node = _DRAWSPACE_NEW_( FaceDrawingNode, FaceDrawingNode( renderer, m_config ) );
        
        node->CreateNoisingTextures();
        node->SetMeshe( SphericalLOD::Body::m_planetpatch_meshe );

        Shader* patch_vshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.vso", true ) );
        Shader* patch_pshader = _DRAWSPACE_NEW_( Shader, Shader( "planethm.pso", true ) );
        patch_vshader->LoadFromFile();
        patch_pshader->LoadFromFile();

        Fx* fx = _DRAWSPACE_NEW_( Fx, Fx );
        fx->AddShader( patch_vshader );
        fx->AddShader( patch_pshader );
        node->SetFx( fx );

        void* tx_data;
        if( false == renderer->CreateTexture( m_collidingheightmap_pass->GetTargetTexture(), &tx_data ) )
        {
            _DSEXCEPTION( "failed to create subpasstarget texture in renderer" );
        }
    
        m_collidingheightmap_pass->GetTargetTexture()->AllocTextureContent();

        ////////////////////////

        m_subpass = m_collidingheightmap_pass;
        m_subpass_node = node;


        std::vector<DrawSpace::SphericalLOD::Patch*> dl;
        // appel handler pour enregistrer et executer la passe
        if( p_handler )
        {
            (*p_handler)( this, 2 );

            m_collidingheightmap_texture = m_collidingheightmap_pass->GetTargetTexture();
            m_collidingheightmap_content = m_collidingheightmap_texture->GetTextureContentPtr();
        }
    }
    p_planetbody->Initialize();
}

Fragment::~Fragment( void )
{    
}

void Fragment::on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch, int p_patch_lod )
{
    m_current_patch = p_patch;
    m_current_patch_lod = p_patch_lod;

    if( m_collisions && m_subpass_node )
    {
        std::vector<DrawSpace::SphericalLOD::Patch*> display_list;
        if( m_current_patch && m_current_patch_lod == 0 )
        {
            display_list.push_back( m_current_patch );

            m_draw_collidinghm = true;

            DrawSpace::SphericalLOD::FaceDrawingNode* node = static_cast<DrawSpace::SphericalLOD::FaceDrawingNode*>( m_subpass_node );
            node->SetDisplayList( display_list );
        }
    }
}

void Fragment::build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap )
{
    for( int y = 0; y < PATCH_RESOLUTION; y++ )
    {
        for( int x = 0; x < PATCH_RESOLUTION; x++ )
        {
            Vertex v, vertex_out;
            int index = ( PATCH_RESOLUTION * y ) + x;

            int index_hm = ( PATCH_RESOLUTION * ( PATCH_RESOLUTION - 1 - y ) ) + x;

            p_patchmeshe.GetVertex( index, v );

            double alt = *( p_heightmap + index_hm );
            alt *= m_config->m_amplitude;
      
            Vector v_out;

            p_patch->ProjectVertex( Vector( v.x, v.y, v.z, 1.0 ), v_out );
            v_out.Scale( m_planetray + alt );

            vertex_out.x = v_out[0];
            vertex_out.y = v_out[1];
            vertex_out.z = v_out[2];

            p_outmeshe.AddVertex( vertex_out );        
        }
    }

    for( long i = 0; i < p_patchmeshe.GetTrianglesListSize(); i++ )
    {
        Triangle t;
        p_patchmeshe.GetTriangles( i, t );
        p_outmeshe.AddTriangle( t );
    }
}

void Fragment::Compute( DrawSpace::Planetoid::Body* p_owner )
{
    if( m_hot )
    {
        Matrix camera_pos;
        bool inject_hotpoint = false;

        if( m_camera )
        {
            dsstring camera_scenename;
            
            DrawSpace::Core::SceneNode<CameraPoint>* camera_node = m_camera->GetOwner();
            camera_node->GetSceneName( camera_scenename );

            p_owner->GetCameraHotpoint( camera_scenename, camera_pos );
            
            inject_hotpoint = true;
        }
        else if( m_inertbody )
        {
            m_inertbody->GetLastLocalWorldTrans( camera_pos );
            inject_hotpoint = true;
        }

        if( inject_hotpoint )
        {
            DrawSpace::Utils::Vector hotpoint;

            hotpoint[0] = camera_pos( 3, 0 );
            hotpoint[1] = camera_pos( 3, 1 );
            hotpoint[2] = camera_pos( 3, 2 );

            m_planetbody->UpdateHotPoint( hotpoint );
            m_planetbody->Compute();
        }
    }
}

void Fragment::SetHotState( bool p_hotstate )
{
    m_hot = p_hotstate;
    m_planetbody->SetHotState( m_hot );
    m_nb_collisionmeshebuild_done = 0;
}

bool Fragment::GetHotState( void )
{
    return m_hot;
}

void Fragment::SetCamera( CameraPoint* p_camera )
{
    m_camera = p_camera;
}

void Fragment::SetInertBody( DrawSpace::Dynamics::InertBody* p_body )
{
    m_inertbody = p_body;
}


CameraPoint* Fragment::GetCamera( void )
{
    return m_camera;
}

InertBody* Fragment::GetInertBody( void )
{
    return m_inertbody;
}

void Fragment::RemoveColliderFromWorld( void )
{
    if( m_collision_state )
    {
        m_collider->RemoveFromWorld();
        m_collider->UnsetKinematic();
        m_collision_state = false;
    }
}

DrawSpace::SphericalLOD::Body* Fragment::GetPlanetBody( void )
{
    return m_planetbody;
}

void Fragment::GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_done )
{
    p_nb_collisionmeshebuild_done = m_nb_collisionmeshebuild_done;
}

void Fragment::UpdateRelativeAlt( dsreal p_alt )
{
    m_planetbody->UpdateRelativeAlt( p_alt );
}

SphericalLOD::Patch* Fragment::GetCurrentPatch( void )
{
    return m_current_patch;
}

int Fragment::GetCurrentPatchLOD( void )
{
    return m_current_patch_lod;
}

DrawSpace::IntermediatePass* Fragment::create_colliding_heightmap_pass( void )
{
    char thisname[32];
    sprintf( thisname, "fragment_%x", this );

    dsstring complete_name = dsstring( thisname ) + dsstring( "_collisionheightmap_pass" );
    IntermediatePass* ipass = _DRAWSPACE_NEW_( IntermediatePass, IntermediatePass( complete_name ) );

    ipass->SetTargetDimsFromRenderer( false );    
    ipass->SetTargetDims( PATCH_RESOLUTION, PATCH_RESOLUTION );
    ipass->SetRenderPurpose( Texture::RENDERPURPOSE_FLOAT32 );

    ipass->Initialize();
    ipass->GetRenderingQueue()->EnableDepthClearing( true );
    ipass->GetRenderingQueue()->EnableTargetClearing( true );
    ipass->GetRenderingQueue()->SetTargetClearingColor( 0, 0, 0, 255 );

    return ipass;
}

void Fragment::DrawSubPass( void )
{
    if( m_draw_collidinghm )
    {
        SubPass::DrawSubPass();
    }
}

void Fragment::SubPassDone( void )
{
    if( m_draw_collidinghm )
    {
        m_collidingheightmap_texture->CopyTextureContent();

        float* heightmap = (float*)m_collidingheightmap_content;
        Meshe final_meshe;
        build_meshe( *( m_planetbody->GetPatcheMeshe() ), m_current_patch, final_meshe, heightmap );

        Dynamics::InertBody::Body::Parameters params;

        params.mass = 0.0;

        params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

        params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
        params.shape_descr.meshe = final_meshe;

        RemoveColliderFromWorld();

        m_collider->SetKinematic( params );
        m_collider->AddToWorld( m_world );

        m_collision_state = true;
        m_nb_collisionmeshebuild_done++;

        m_draw_collidinghm = false;
    }
}
