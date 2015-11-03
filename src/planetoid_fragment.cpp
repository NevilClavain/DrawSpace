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

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planetoid;
using namespace DrawSpace::Dynamics;

Fragment::Fragment( DrawSpace::SphericalLOD::Body* p_planetbody, Collider* p_collider, dsreal p_planetray, bool p_collisions ) :
m_planetbody( p_planetbody ), 
m_collider( p_collider ),
m_suspend_update( false ),
m_collision_state( false ),
m_planetray( p_planetray ),
m_hot( false ),
m_camera( NULL ),
m_inertbody( NULL ),
m_collisions( p_collisions ),
m_nb_collisionmeshebuild_req( 0 ),
m_nb_collisionmeshebuild_done( 0 ),
m_nb_collisionmeshebuild_added( 0 ),
m_current_patch( NULL )
{
    if( m_collisions )
    {

        m_patch_update_cb = _DRAWSPACE_NEW_( PatchUpdateCb, PatchUpdateCb( this, &Fragment::on_patchupdate ) );
        m_planetbody->RegisterPatchUpdateHandler( m_patch_update_cb );

        m_runnercb = _DRAWSPACE_NEW_( RunnerMsgCb, RunnerMsgCb( this, &Fragment::on_meshebuild_request ) );
        m_runnerevt = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &Fragment::on_meshebuild_result ) );

        m_runner = _DRAWSPACE_NEW_( Runner, Runner );
              
        m_runner->RegisterTaskMsgHandler( m_runnercb );
        m_runner->RegisterEventHandler( m_runnerevt );

        m_runner->Startup();
    }
    p_planetbody->Initialize();
}

Fragment::~Fragment( void )
{
    _DRAWSPACE_DELETE_( m_runner );    
}

void Fragment::on_meshebuild_request( PropertyPool* p_args )
{
    if( !m_collisions )
    {
        return;
    }

    //localy copy inputs

    DrawSpace::Core::Meshe patchmeshe;

    patchmeshe = *( p_args->GetPropValue<Meshe*>( "patchmeshe" ) );

    SphericalLOD::Patch* patch = p_args->GetPropValue<SphericalLOD::Patch*>( "patch" );

    ////////////////////////////// do the work

    Meshe final_meshe;
    build_meshe( patchmeshe, patch, final_meshe );

    m_nb_collisionmeshebuild_done++;


    Dynamics::InertBody::Body::Parameters params;

    params.mass = 0.0;

    params.initial_attitude.Translation( 0.0, 0.0, 0.0 );

    params.shape_descr.shape = DrawSpace::Dynamics::Body::MESHE_SHAPE;
    params.shape_descr.meshe = final_meshe;

    

    ////////////////////////////////////////////

    m_meshe_ready_mutex.WaitInfinite();
    m_params = params;
    m_meshe_ready = true;
    m_meshe_ready_mutex.Release();

    Sleep( 25 );
}

void Fragment::on_meshebuild_result( DrawSpace::Core::Runner::State p_runnerstate )
{
    if( p_runnerstate == DrawSpace::Core::Runner::TASK_DONE )
    {
        // completer ici
        // ...

        m_runner->ResetState();
    }
}

void Fragment::on_spherelod_event( DrawSpace::SphericalLOD::Body* p_body, int p_currentface )
{
    if( !m_collisions )
    {
        return;
    }

    long tri_index = 0;
    dsreal alt = p_body->GetHotPointAltitud();

    if( alt < 9000.0 )
    {

        m_suspend_update = true;

        m_meshe_ready_mutex.WaitInfinite();
        m_meshe_ready = false;
        m_meshe_ready_mutex.Release();

        SphericalLOD::Patch* curr_patch = p_body->GetFaceCurrentLeaf( p_currentface );

        PropertyPool props;
        props.AddPropValue<Meshe*>( "patchmeshe", p_body->GetPatcheMeshe() );
        props.AddPropValue<SphericalLOD::Patch*>( "patch", curr_patch );
        //m_buildmeshereq_msg->PushMessage( props );
        m_runner->PushMessage( props );

        m_nb_collisionmeshebuild_req++;
    }
    else
    {
        if( m_collision_state )
        {
            if( !m_suspend_update )
            {
                m_collider->RemoveFromWorld();
                m_collider->UnsetKinematic();
            }
            m_collision_state = false;
        }
    }

}

void Fragment::on_patchupdate( DrawSpace::SphericalLOD::Patch* p_patch )
{
    m_current_patch = p_patch;
}

void Fragment::build_meshe( DrawSpace::Core::Meshe& p_patchmeshe, SphericalLOD::Patch* p_patch, DrawSpace::Core::Meshe& p_outmeshe )
{
    for( long i = 0; i < p_patchmeshe.GetVertexListSize(); i++ )
    {                
        Vertex v, vertex_out;
        p_patchmeshe.GetVertex( i, v );
      
        Vector v_out;

        p_patch->ProjectVertex( Vector( v.x, v.y, v.z, 1.0 ), v_out );
        v_out.Scale( m_planetray );
        vertex_out.x = v_out[0];
        vertex_out.y = v_out[1];
        vertex_out.z = v_out[2];

        p_outmeshe.AddVertex( vertex_out );
    }

    for( long i = 0; i < p_patchmeshe.GetTrianglesListSize(); i++ )
    {
        Triangle t;
        p_patchmeshe.GetTriangles( i, t );
        p_outmeshe.AddTriangle( t );
    }
}

void Fragment::Update( World* p_world, DrawSpace::Planetoid::Body* p_owner )
{
    if( m_suspend_update )
    {
        bool read_status = m_meshe_ready_mutex.Wait( 0 );

        if( read_status )
        {
            bool meshe_ready = m_meshe_ready;

            Dynamics::InertBody::Body::Parameters params = m_params;
            m_meshe_ready_mutex.Release();

            if( meshe_ready )
            {
                
                if( m_collision_state )
                {
                    m_collider->RemoveFromWorld();
                    m_collider->UnsetKinematic();
                }
                

                // bullet meshe build done

                m_collider->SetKinematic( params );
                m_collider->AddToWorld( p_world );
                m_collision_state = true;
                m_suspend_update = false;

                m_nb_collisionmeshebuild_added++;
            }
        }
    }

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
        if( !m_suspend_update )
        {
            m_collider->RemoveFromWorld();
            m_collider->UnsetKinematic();
        }
        m_collision_state = false;
    }
}

DrawSpace::SphericalLOD::Body* Fragment::GetPlanetBody( void )
{
    return m_planetbody;
}

void Fragment::GetCollisionMesheBuildStats( long& p_nb_collisionmeshebuild_req, long& p_nb_collisionmeshebuild_done, long& p_nb_collisionmeshebuild_added )
{
    p_nb_collisionmeshebuild_req = m_nb_collisionmeshebuild_req;
    p_nb_collisionmeshebuild_done = m_nb_collisionmeshebuild_done;
    p_nb_collisionmeshebuild_added = m_nb_collisionmeshebuild_added;
}

void Fragment::UpdateRelativeAlt( dsreal p_alt )
{
    m_planetbody->UpdateRelativeAlt( p_alt );
}

SphericalLOD::Patch* Fragment::GetCurrentPatch( void )
{
    return m_current_patch;
}