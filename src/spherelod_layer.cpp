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

#include "spherelod_layer.h"
#include "spherelod_root.h"
#include "spherelod_drawing.h"
#include "renderer.h"
#include "plugin.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Dynamics;
using namespace DrawSpace::SphericalLOD;

Layer::Layer( Dynamics::Orbiter* p_owner, DrawSpace::SphericalLOD::Config* p_config, DrawSpace::Dynamics::World* p_world, DrawSpace::SphericalLOD::Body* p_planetbody, 
                Collider* p_collider, Layer::SubPassCreationHandler* p_handler, int p_index ) :
m_world( p_world ),
m_config( p_config ),
m_planetbody( p_planetbody ), 
m_collider( p_collider ),
m_collision_state( false ),
m_hot( false ),
m_camera( NULL ),
m_inertbody( NULL ),
m_nb_collisionmeshebuild_done( 0 ),
m_current_patch( NULL ),
m_draw_collidinghm( false ),
m_handler( p_handler ),
m_current_collisions_hm( NULL ),
m_owner( p_owner )
{
    m_collisions = m_config->m_layers_descr[p_index].enable_collisions;
    m_planetray = 1000.0 * m_config->m_layers_descr[p_index].ray;

    if( m_collisions )
    {
        m_patch_update_cb = _DRAWSPACE_NEW_( PatchUpdateCb, PatchUpdateCb( this, &Layer::on_patchupdate ) );
        m_planetbody->RegisterPatchUpdateHandler( m_patch_update_cb );

        for( int i = 0; i < 6; i++ )
        {
            m_collisions_hms[i] = _DRAWSPACE_NEW_( Collisions, Collisions( this, p_config, i, p_index ) );
            m_collisions_hms[i]->Disable();        
        }
    }
    p_planetbody->Initialize();
}

Layer::~Layer( void )
{    
}

Layer::SubPassCreationHandler* Layer::GetSubPassCreationHandler( void )
{
    return m_handler;
}

void Layer::on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch, int p_patch_lod )
{
    m_current_patch = p_patch;

    if( m_collisions )
    {
        std::vector<DrawSpace::SphericalLOD::Patch*> display_list;
        if( m_current_patch && p_patch_lod == 0 )
        {
            display_list.push_back( m_current_patch );

            if( p_patch->GetOrientation() == m_planetbody->GetCurrentFace() )
            {
                // ce patch appartient bien a la face "courante"

                m_draw_collidinghm = true;

                m_current_collisions_hm = m_collisions_hms[p_patch->GetOrientation()];
                m_current_collisions_hm->Enable();
               
                DrawSpace::SphericalLOD::FaceDrawingNode* node = static_cast<DrawSpace::SphericalLOD::FaceDrawingNode*>( m_current_collisions_hm->GetNode() );
                node->SetDisplayList( display_list );
            }
        }
    }
}

void Layer::build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe, float* p_heightmap )
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

void Layer::Compute( Root* p_owner )
{
    // ECH 9/4/2016 modif algo

    if( m_camera )
    {
        SceneNode<CameraPoint>* camera_node = m_camera->GetOwner();

        if( m_hot )
        {
            // si hot, c'est une camera de type FREE_ON_PLANET
            // donc faire un UpdateHotPoint

            Matrix res;
            
            Orbiter* orbiter = static_cast<Orbiter*>( m_camera->GetReferentBody() );

            res.Identity();
            camera_node->GetTransformationRelativeTo( orbiter->GetOwner(), res );
            Vector pos;

            pos[0] = res( 3, 0 );
            pos[1] = res( 3, 1 );
            pos[2] = res( 3, 2 );
            pos[3] = 1.0;
       
            m_planetbody->UpdateHotPoint( pos );
        }

        //////////////////////////////////////////////////////////////////
        // calcul invariant relative pos

        Matrix pos_planet;
        Matrix pos_cam;

        SceneNode<DrawSpace::Dynamics::Orbiter>* orbiter_node = m_owner->GetOwner();


        orbiter_node->GetFinalTransform( pos_planet );
        camera_node->GetFinalTransform( pos_cam );

        Vector delta;

        delta[0] = pos_cam( 3, 0 ) - pos_planet( 3, 0 );
        delta[1] = pos_cam( 3, 1 ) - pos_planet( 3, 1 );
        delta[2] = pos_cam( 3, 2 ) - pos_planet( 3, 2 );
        delta[3] = 1.0;

        m_planetbody->UpdateInvariantViewerPos( delta );

    }
    else if( m_inertbody )
    {        
        if( m_hot )
        {
            m_planetbody->UpdateHotPoint( m_relative_hotviewerpos ); // m_relative_hotviewerpos est mis à jour seulement 
                                                                  // quand on est relatif (hot)
            m_planetbody->Compute();            
        }

        //////////////////////////////////////////////////////////////////
        // calcul invariant relative pos

        Matrix pos_planet;
        Matrix pos_body;

        SceneNode<DrawSpace::Dynamics::InertBody>* inertbody_node = m_inertbody->GetOwner();
        SceneNode<DrawSpace::Dynamics::Orbiter>* orbiter_node = m_owner->GetOwner();


        orbiter_node->GetFinalTransform( pos_planet );
        inertbody_node->GetFinalTransform( pos_body );

        Vector delta;

        delta[0] = pos_body( 3, 0 ) - pos_planet( 3, 0 );
        delta[1] = pos_body( 3, 1 ) - pos_planet( 3, 1 );
        delta[2] = pos_body( 3, 2 ) - pos_planet( 3, 2 );
        delta[3] = 1.0;

        m_planetbody->UpdateInvariantViewerPos( delta );
    }

    /*
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
    */
}

void Layer::SetHotState( bool p_hotstate )
{
    m_hot = p_hotstate;
    m_planetbody->SetHotState( m_hot );
    m_nb_collisionmeshebuild_done = 0;
}

bool Layer::GetHotState( void )
{
    return m_hot;
}

void Layer::SetCamera( CameraPoint* p_camera )
{
    m_camera = p_camera;
}

void Layer::SetInertBody( DrawSpace::Dynamics::InertBody* p_body )
{
    m_inertbody = p_body;
}


CameraPoint* Layer::GetCamera( void )
{
    return m_camera;
}

InertBody* Layer::GetInertBody( void )
{
    return m_inertbody;
}

void Layer::RemoveColliderFromWorld( void )
{
    if( m_collision_state )
    {
        m_collider->RemoveFromWorld();
        m_collider->UnsetKinematic();
        m_collision_state = false;
    }
}

DrawSpace::SphericalLOD::Body* Layer::GetPlanetBody( void )
{
    return m_planetbody;
}

void Layer::GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_done )
{
    p_nb_collisionmeshebuild_done = m_nb_collisionmeshebuild_done;
}

void Layer::UpdateRelativeAlt( dsreal p_alt )
{
    m_planetbody->UpdateRelativeAlt( p_alt );
}

void Layer::SubPassDone( DrawSpace::SphericalLOD::Collisions* p_collider )
{
    if( m_draw_collidinghm )
    {        
        m_current_collisions_hm->GetHMTexture()->CopyTextureContent();

        float* heightmap = (float*)m_current_collisions_hm->GetHMTextureContent();

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

        //m_collisions_hm->Disable();
        m_current_collisions_hm->Disable();
        m_current_collisions_hm = NULL;
    }
}

void Layer::ResetPlanetBody( void )
{
    m_planetbody->Reset();
}

void Layer::UpdateRelativeHotViewerPos( const Utils::Vector& p_pos )
{
    m_relative_hotviewerpos = p_pos;
}

