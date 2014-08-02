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

#include "planetoid_fragment.h"
#include "planetoid_body.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;
using namespace DrawSpace::Planetoid;
using namespace DrawSpace::Dynamics;

Fragment::Fragment( const dsstring& p_name, DrawSpace::SphericalLOD::Body* p_planetbody, Collider* p_collider, dsreal p_planetray, bool p_collisions ) :
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
m_nb_collisionmeshebuild_added( 0 )
{
    m_name = p_name;

    if( m_collisions )
    {

        DrawSpace::Core::Mediator* mediator = Mediator::GetInstance();

        m_runner = _DRAWSPACE_NEW_( Runner, Runner );

        m_spherelod_evt_cb = _DRAWSPACE_NEW_( SphereLODEvtCb, SphereLODEvtCb( this, &Fragment::on_spherelod_event ) );
        m_planetbody->RegisterEventHandler( m_spherelod_evt_cb );

        //m_runner_evt_cb = _DRAWSPACE_NEW_( RunnerEvtCb, RunnerEvtCb( this, &Fragment::on_meshebuild_request ) );
        m_runner_msg_cb = _DRAWSPACE_NEW_( RunnerMsgCb, RunnerMsgCb( this, &Fragment::on_meshebuild_request ) );
       
        //dsstring reqevtname = p_name + dsstring( "_ReqBuildMesheEvent" );
        dsstring reqname = p_name + dsstring( "_ReqBuildMesheEvent" );

        //m_buildmeshe_event = mediator->CreateEvent( reqevtname );

        m_buildmeshereq_msg = mediator->CreateMessageQueue( reqname );
        
        /*
        m_buildmeshe_event->args->AddProp<Meshe*>( "patchmeshe" );
        m_buildmeshe_event->args->AddProp<dsreal>( "sidelength" );
        m_buildmeshe_event->args->AddProp<dsreal>( "xpos" );
        m_buildmeshe_event->args->AddProp<dsreal>( "ypos" );
        m_buildmeshe_event->args->AddProp<int>( "orientation" );
        */

        //m_runner->RegisterEventHandler( m_buildmeshe_event, m_runner_evt_cb );

        m_runner->RegisterMsgHandler( m_buildmeshereq_msg, m_runner_msg_cb );

        m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
        m_task->Startup( m_runner );

    }
}

Fragment::~Fragment( void )
{
    _DRAWSPACE_DELETE_( m_runner );
    _DRAWSPACE_DELETE_( m_task );
}

void Fragment::on_meshebuild_request( PropertyPool* p_args )
{
    if( !m_collisions )
    {
        return;
    }

    //localy copy inputs

    DrawSpace::Core::Meshe patchmeshe;
    int patch_orientation;
    dsreal sidelength;
    dsreal xpos, ypos;


    patchmeshe = *( p_args->GetPropValue<Meshe*>( "patchmeshe" ) );
    patch_orientation = p_args->GetPropValue<int>( "orientation" );
    sidelength = p_args->GetPropValue<dsreal>( "sidelength" );
    xpos = p_args->GetPropValue<dsreal>( "xpos" );
    ypos = p_args->GetPropValue<dsreal>( "ypos" );


    ////////////////////////////// do the work

    Meshe final_meshe;
    build_meshe( patchmeshe, patch_orientation, sidelength, xpos, ypos, final_meshe );

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

void Fragment::on_spherelod_event( DrawSpace::SphericalLOD::Body* p_body, int p_currentface )
{
    if( !m_collisions )
    {
        return;
    }

    long tri_index = 0;
    dsreal alt = p_body->GetAltitud();

    if( alt < 9000.0 )
    {
        /*
        if( !m_suspend_update )
        {
            SphericalLOD::Patch* curr_patch = p_body->GetFaceCurrentLeaf( p_currentface );

            dsreal xpos, ypos;
            curr_patch->GetPos( xpos, ypos );


            if( m_collision_state )
            {
                m_collider->RemoveFromWorld();
                m_collider->UnsetKinematic();
            }

            m_suspend_update = true;

            m_meshe_ready_mutex.WaitInfinite();
            m_meshe_ready = false;
            m_meshe_ready_mutex.Release();

            PropertyPool props;

            props.AddPropValue<Meshe*>( "patchmeshe", p_body->GetPatcheMeshe() );
            props.AddPropValue<dsreal>( "sidelength", curr_patch->GetSideLength() / m_planetray );
            props.AddPropValue<dsreal>( "xpos", xpos / m_planetray );
            props.AddPropValue<dsreal>( "ypos", ypos / m_planetray );
            props.AddPropValue<int>( "orientation", curr_patch->GetOrientation() );

            m_buildmeshereq_msg->PushMessage( props );

            m_nb_collisionmeshebuild_req++;

            ////////////////////////////////////////////////
        }
        */

        m_suspend_update = true;

        m_meshe_ready_mutex.WaitInfinite();
        m_meshe_ready = false;
        m_meshe_ready_mutex.Release();


        SphericalLOD::Patch* curr_patch = p_body->GetFaceCurrentLeaf( p_currentface );

        dsreal xpos, ypos;
        curr_patch->GetPos( xpos, ypos );

        PropertyPool props;

        props.AddPropValue<Meshe*>( "patchmeshe", p_body->GetPatcheMeshe() );
        props.AddPropValue<dsreal>( "sidelength", curr_patch->GetSideLength() / m_planetray );
        props.AddPropValue<dsreal>( "xpos", xpos / m_planetray );
        props.AddPropValue<dsreal>( "ypos", ypos / m_planetray );
        props.AddPropValue<int>( "orientation", curr_patch->GetOrientation() );

        m_buildmeshereq_msg->PushMessage( props );

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

void Fragment::build_meshe( Meshe& p_patchmeshe, int p_patch_orientation, dsreal p_sidelength, dsreal p_xpos, dsreal p_ypos, Meshe& p_outmeshe )
{
    for( long i = 0; i < p_patchmeshe.GetVertexListSize(); i++ )
    {                

        Vertex v, v2, v3;
        p_patchmeshe.GetVertex( i, v );

        v.x = v.x * p_sidelength / 2.0;
        v.y = v.y * p_sidelength / 2.0;
        v.z = v.z * p_sidelength / 2.0;

        v.x += p_xpos;
        v.y += p_ypos;

        switch( p_patch_orientation )
        {
            case SphericalLOD::Patch::FrontPlanetFace:

                v2.x = v.x;
                v2.y = v.y;
                v2.z = 1.0;
                break;

            case SphericalLOD::Patch::RearPlanetFace:

                v2.x = -v.x;
                v2.y = v.y;
                v2.z = -1.0;
                break;

            case SphericalLOD::Patch::LeftPlanetFace:

                v2.x = -1.0;
                v2.y = v.y;
                v2.z = v.x;
                break;

            case SphericalLOD::Patch::RightPlanetFace:

                v2.x = 1.0;
                v2.y = v.y;
                v2.z = -v.x;
                break;

            case SphericalLOD::Patch::TopPlanetFace:

                v2.x = v.x;
                v2.y = 1.0;
                v2.z = -v.y;
                break;

            case SphericalLOD::Patch::BottomPlanetFace:

                v2.x = v.x;
                v2.y = -1.0;
                v2.z = v.y;
                break;
        }

        dsreal xtemp = v2.x;
        dsreal ytemp = v2.y;
        dsreal ztemp = v2.z;

        v2.x = xtemp * sqrt( 1.0 - ytemp * ytemp * 0.5 - ztemp * ztemp * 0.5 + ytemp * ytemp * ztemp * ztemp / 3.0 );
        v2.y = ytemp * sqrt( 1.0 - ztemp * ztemp * 0.5 - xtemp * xtemp * 0.5 + xtemp * xtemp * ztemp * ztemp / 3.0 );
        v2.z = ztemp * sqrt( 1.0 - xtemp * xtemp * 0.5 - ytemp * ytemp * 0.5 + xtemp * xtemp * ytemp * ytemp / 3.0 );

        v3.x = v2.x * m_planetray;
        v3.y = v2.y * m_planetray;
        v3.z = v2.z * m_planetray;

        p_outmeshe.AddVertex( v3 );
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
    /*
    if( m_suspend_update )
    {
        bool read_status = m_meshe_ready_mutex.Wait( 0 );

        if( read_status )
        {
            bool meshe_ready = m_meshe_ready;
            m_meshe_ready_mutex.Release();

            if( meshe_ready )
            {
                // bullet meshe build done
                m_collider->AddToWorld( p_world );
                m_collision_state = true;
                m_suspend_update = false;

                m_nb_collisionmeshebuild_added++;
            }
        }
    }
    */


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
            dsstring camera_name;

            m_camera->GetName( camera_name );
            p_owner->GetCameraHotpoint( camera_name, camera_pos );
            
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