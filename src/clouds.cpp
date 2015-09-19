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

#include <algorithm>
#include "clouds.h"
#include "memalloc.h"

using namespace DrawSpace;
using namespace DrawSpace::Core;
using namespace DrawSpace::Utils;

Clouds::Clouds( void ) : 
m_runner_state( 0 ),
m_update_clouds_meshes( false ),
m_clouds_sort_request( false ),
m_current_camera( NULL ),
m_previous_camera_pos_avail( false ),
m_recompute_count( 0 ),
m_sort_running( false ),
m_owner( NULL ),
m_sorting_distance( 1000.0 ),
m_details( true ),
m_running( false )
{
    m_proceduralcb = _DRAWSPACE_NEW_( ProceduralCb, ProceduralCb( this, &Clouds::on_procedural ) );
    m_cameracb = _DRAWSPACE_NEW_( CameraEventCb, CameraEventCb( this, &Clouds::on_camera_event ) );
    m_runnercb = _DRAWSPACE_NEW_( RunnerMsgCb, RunnerMsgCb( this, &Clouds::on_sort_request ) );

    m_runner = _DRAWSPACE_NEW_( Runner, Runner );
    DrawSpace::Core::Mediator* mediator = Mediator::GetInstance();
    m_sort_msg = mediator->CreateMessageQueue( "ReqSortEvent" );
    
    m_runner->RegisterMsgHandler( m_sort_msg, m_runnercb );

    m_task = _DRAWSPACE_NEW_( Task<Runner>, Task<Runner> );
    m_task->Startup( m_runner );
}

Clouds::~Clouds( void )
{
}

void Clouds::on_sort_request( PropertyPool* p_args )
{
    Matrix ImpostorMat, CamMat;
    ImpostorMat = p_args->GetPropValue<Matrix>( "ImpostorMat" );
    CamMat = p_args->GetPropValue<Matrix>( "CamMat" );

    m_sort_run_mutex.WaitInfinite();

    execsortz( ImpostorMat, CamMat );

    update_from_clouds();

    m_sort_run_mutex.Release();

    m_runner_state_mutex.WaitInfinite();
    m_runner_state = 0;
    m_runner_state_mutex.Release();
}

void Clouds::on_camera_event( SceneNodeGraph::CameraEvent p_event, BaseSceneNode* p_node )
{
    if( DrawSpace::Core::SceneNodeGraph::ACTIVE == p_event )
    {
        DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* prec_camera = m_current_camera;        
        m_current_camera = static_cast< DrawSpace::Core::SceneNode<DrawSpace::Dynamics::CameraPoint>* >( p_node );

        if( prec_camera != m_current_camera )
        {
            m_clouds_sort_request = true;
        }
    }
}

void Clouds::on_procedural( Procedural::Atomic* p_atom )
{
    // decode procedural rules messages
    Procedural::Array* message = static_cast<Procedural::Array*>( p_atom );
    Procedural::String* opcode = static_cast<Procedural::String*>( message->GetValueAt( 0 ) );

    if( "declare_cloud" == opcode->GetValue() )
    {
        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_cloudposz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_cloudposx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_cloudposz->GetResultValue() );


        m_new_cloud = new CloudUnitDescriptor;

        m_new_cloud->position[0] = posx->GetValue();
        m_new_cloud->position[1] = 0.0;
        m_new_cloud->position[2] = posz->GetValue();
        m_new_cloud->position[3] = 1.0;

    }
    else if( "add_core_impostor" == opcode->GetValue() )
    {
        ImpostorsDisplayListEntry idle;

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        //Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( message->GetValueAt( 3 ) );

        Procedural::Index* impostor_uvindex = static_cast<Procedural::Index*>( message->GetValueAt( 3 ) );
        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( impostor_uvindex->GetResultValue() );


        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );
        

        Procedural::Real* scale = static_cast<Procedural::Real*>( message->GetValueAt( 4 ) );

        idle.width_scale = scale->GetValue();
        idle.height_scale = scale->GetValue();

        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];


        idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
        idle.localpos[1] = m_new_cloud->position[1];
        idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
    
        m_new_cloud->impostors.push_back( idle );


    }
    else if( "add_bottom_impostor" == opcode->GetValue() )
    {
        ImpostorsDisplayListEntry idle;

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 1 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 3 ) );

        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( message->GetValueAt( 4 ) );

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );
        Procedural::Real* posy = static_cast<Procedural::Real*>( message->GetValueAt( 2 ) );

        Procedural::Real* scale = static_cast<Procedural::Real*>( message->GetValueAt( 5 ) );

        idle.width_scale = scale->GetValue();
        idle.height_scale = scale->GetValue();

        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];


        idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
        idle.localpos[1] = m_new_cloud->position[1] + posy->GetValue();
        idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
    
        m_new_cloud->impostors.push_back( idle );
    
    }
    else if( "add_impostor" == opcode->GetValue() && m_details )
    {
        ImpostorsDisplayListEntry idle;

        
        Procedural::Index* impostor_uvindex = static_cast<Procedural::Index*>( message->GetValueAt( 1 ) );
        Procedural::Vector* impostor_uv = static_cast<Procedural::Vector*>( impostor_uvindex->GetResultValue() );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostorscale = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 2 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posx = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 3 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posy = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 4 ) );

        Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>* rand_impostor_posz = 
            static_cast<Procedural::RandomDistribution<dsreal, std::uniform_real_distribution<dsreal>, Procedural::Real>*>( message->GetValueAt( 5 ) );



        Procedural::Real* impostor_scale = static_cast<Procedural::Real*>( rand_impostorscale->GetResultValue() );

        idle.width_scale = impostor_scale->GetValue();
        idle.height_scale = impostor_scale->GetValue();

        Procedural::Real* posx = static_cast<Procedural::Real*>( rand_impostor_posx->GetResultValue() );
        Procedural::Real* posy = static_cast<Procedural::Real*>( rand_impostor_posy->GetResultValue() );
        Procedural::Real* posz = static_cast<Procedural::Real*>( rand_impostor_posz->GetResultValue() );

        
        idle.u1 = impostor_uv->GetValue()[0];
        idle.v1 = impostor_uv->GetValue()[1];
        idle.u2 = impostor_uv->GetValue()[2];
        idle.v2 = impostor_uv->GetValue()[1];
        idle.u3 = impostor_uv->GetValue()[2];
        idle.v3 = impostor_uv->GetValue()[3];
        idle.u4 = impostor_uv->GetValue()[0];
        idle.v4 = impostor_uv->GetValue()[3];
        

        idle.localpos[0] = m_new_cloud->position[0] + posx->GetValue();
        idle.localpos[1] = m_new_cloud->position[1] + posy->GetValue();
        idle.localpos[2] = m_new_cloud->position[2] + posz->GetValue();
    
        m_new_cloud->impostors.push_back( idle );
    
    }
    else if( "push_cloud" == opcode->GetValue() )
    {
        m_clouds.push_back( m_new_cloud );
    }
}

void Clouds::execsortz( const DrawSpace::Utils::Matrix& p_impostor_mat, const DrawSpace::Utils::Matrix& p_cam_mat )
{
    // compute all camera-space z-depth

    m_runner_state_mutex.WaitInfinite();
    m_runner_state = 1;
    m_runner_state_mutex.Release();

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        Matrix local_trans;

        local_trans.Translation( m_clouds[i]->position );

        Matrix final = local_trans * p_impostor_mat;

        Matrix cam_mat = p_cam_mat;

        Vector point_imp( 0.0, 0.0, 0.0, 1.0 );
        Vector point_cam( 0.0, 0.0, 0.0, 1.0 );

        Vector t_point_imp, t_point_cam;

        final.Transform( &point_imp, &t_point_imp );        
        cam_mat.Transform( &point_cam, &t_point_cam );

        // la distance entre l'impostor et le point camera;
        Vector dist_imp;
        
        dist_imp[0] = t_point_imp[0] - t_point_cam[0];
        dist_imp[1] = t_point_imp[1] - t_point_cam[1];
        dist_imp[2] = t_point_imp[2] - t_point_cam[2];
        dist_imp[3] = 1.0;

        m_clouds[i]->distToView = dist_imp.Length();
    }

    m_runner_state_mutex.WaitInfinite();
    m_runner_state = 2;
    m_runner_state_mutex.Release();

    std::sort( m_clouds.begin(), m_clouds.end(), Clouds::nodes_comp );
}


bool Clouds::nodes_comp( CloudUnitDescriptor* p_n1, CloudUnitDescriptor* p_n2 )
{
    return ( p_n1->distToView > p_n2->distToView );
}

void Clouds::impostors_init( void )
{
    m_idl.clear();

    m_runner_state_mutex.WaitInfinite();
    m_runner_state = 3;
    m_runner_state_mutex.Release();

    for( size_t i = 0; i < m_clouds.size(); i++ )
    {
        for( size_t j = 0; j < m_clouds[i]->impostors.size(); j++ )
        {     
            m_idl.push_back( m_clouds[i]->impostors[j] );
        }
    }    
}

void Clouds::update_from_clouds( void )
{
    impostors_init();
   
    m_update_mutex.WaitInfinite();
    m_update_clouds_meshes = true;
    m_update_mutex.Release();
}

void Clouds::ImpostorsInit( void )
{
    m_clouds_sort_request = true;
    impostors_init();
    Chunk::ImpostorsInit();
}

int Clouds::GetRunnerState( void )
{
    return m_runner_state;
}

Clouds::ProceduralCb* Clouds::GetProceduralCallback( void )
{
    return m_proceduralcb;
}

void Clouds::OnRegister( DrawSpace::Core::SceneNodeGraph* p_scenegraph, DrawSpace::Core::BaseSceneNode* p_node )
{
    Chunk::OnRegister( p_scenegraph, p_node );
    m_scenenodegraph->RegisterCameraEvtHandler( m_cameracb );
    m_owner = p_node;
}

void Clouds::Update2( DrawSpace::Utils::TimeManager& p_timemanager )
{
    if( !m_owner )
    {
        return;
    }

    m_running = true;

    Chunk::Update2( p_timemanager );

    m_update_mutex.WaitInfinite();
    if( m_update_clouds_meshes )
    {
        ImpostorsUpdate();
        m_update_clouds_meshes = false;
    }
    m_update_mutex.Release();

    if( m_current_camera )
    {
        Matrix CamMat;
        
        m_current_camera->GetFinalTransform( CamMat );
       
        Vector current_camera_pos;

        current_camera_pos[0] = CamMat( 3, 0 );
        current_camera_pos[1] = CamMat( 3, 1 );
        current_camera_pos[2] = CamMat( 3, 2 );
        current_camera_pos[3] = 1.0;

        if( m_previous_camera_pos_avail )
        {
            Vector delta_cam_pos;

            delta_cam_pos[0] = current_camera_pos[0] - m_previous_camera_pos[0];
            delta_cam_pos[0] = current_camera_pos[1] - m_previous_camera_pos[1];
            delta_cam_pos[0] = current_camera_pos[2] - m_previous_camera_pos[2];
            delta_cam_pos[3] = 1.0;

            if( delta_cam_pos.Length() > m_sorting_distance )
            {
                m_clouds_sort_request = true;
                m_previous_camera_pos = current_camera_pos;
            }
        }
        else
        {
            m_previous_camera_pos = current_camera_pos;
            m_previous_camera_pos_avail = true;
        }
    }

    bool busy = true;
    if( m_sort_run_mutex.Wait( 0 ) )
    {
        busy = m_sort_running;
    }
    m_sort_run_mutex.Release();

    if( m_clouds_sort_request )
    {
        if( !busy )
        {
            // get clouds node global transform
            Matrix ImpostorMat;
            m_owner->GetFinalTransform( ImpostorMat );

            Matrix CamMat;
            m_current_camera->GetFinalTransform( CamMat );

            PropertyPool props;

            props.AddPropValue<Matrix>( "ImpostorMat", ImpostorMat );
            props.AddPropValue<Matrix>( "CamMat", CamMat );

            m_sort_msg->PushMessage( props );
            m_recompute_count++;            
        }
        m_clouds_sort_request = false;
    }    
}

void Clouds::EnableDetails( bool p_details )
{
    m_details = p_details;
}